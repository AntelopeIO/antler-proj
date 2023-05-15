#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include <cmath>
#include <string>
#include <string_view>
#include <sstream>
#include <iostream>
#include <optional>
#include <array>
#include <memory>
#include <stdexcept>
#include "../system/utils.hpp"

#include "yaml.hpp"


namespace antler::project
{

/// Simple class to encapsulate a project version.
class version
{
public:
   using self = version; ///< Alias for self type.

   /// @param ver  A string to create this version with.
   explicit inline version(std::string_view ver)
   {
      system::info_log("project.version {0}", ver);
      ANTLER_CHECK(parse(ver) >= 0, "version malformed");
   }

   /// @param maj  Major version component.
   /// @param min  Minor version component.
   /// @param pat  Patch version component.
   /// @param tweak  Tweak version component.
   explicit inline version(uint16_t maj = 0, uint16_t min = 0, uint16_t pat = 0, std::string tweak = "")
      : major_comp(maj)
      , minor_comp(min)
      , patch_comp(pat)
      , tweak_comp(std::move(tweak))
   {}

   /// Copy constructor.
   /// @param rhs  Source to copy from
   version(const self& rhs) = default;

   /// @param ver  A string to create this version with. ver is evaluated to see if it might be a semver.
   inline version& operator=(std::string_view ver)
   {
      ANTLER_CHECK(parse(ver) >= 0, "version malformed");
      return *this;
   }

   /// @param rhs  Source to copy from
   version& operator=(const self& rhs) = default;

   /// comparison operator
   /// @param rhs  The right hand side semver to compare against.
   /// @return Follows standard rules.
   //[[nodiscard]] std::strong_ordering operator<=>(const self& rhs) const;
   [[nodiscard]] inline bool operator<(const self& rhs) const noexcept { return compare(rhs) == -1; }
   [[nodiscard]] inline bool operator<=(const self& rhs) const noexcept { return compare(rhs) != 1; }
   [[nodiscard]] inline bool operator>(const self& rhs) const noexcept { return compare(rhs) == 1; }
   [[nodiscard]] inline bool operator>=(const self& rhs) const noexcept { return compare(rhs) != -1; }

   // comparison operators:
   [[nodiscard]] inline bool operator==(const self& rhs) const noexcept { return compare(rhs) == 0; }
   [[nodiscard]] inline bool operator!=(const self& rhs) const noexcept { return compare(rhs) != 0; }

   /// Clear any values.
   inline void clear() noexcept
   {
      major_comp = 0;
      minor_comp = 0;
      patch_comp = 0;
      tweak_comp = "";
   }

   /// @return true if this version is empty.
   [[nodiscard]] inline bool empty() const noexcept
   {
      return major() == 0 &&
             minor() == 0 &&
             patch() == 0 &&
             tweak().empty();
   }

   /// @return The string this version was built from.
   [[nodiscard]] std::string to_string() const noexcept
   {
      std::string ret = std::to_string(major_comp) + "." + std::to_string(minor_comp) + "." + std::to_string(patch_comp);
      if (!tweak_comp.empty())
         ret += "-" + tweak_comp;
      return ret;
   }

   [[nodiscard]] static inline int64_t parse_component(uint16_t& c, std::string_view s, bool allow_v = false)
   {
      int64_t consumed = 0;
      if (allow_v)
      {
         if (s[0] == 'v' || s[0] == 'V')
         {
            s = s.substr(1);
            consumed++;
         }
      }

      if (s[0] >= '0' && s[0] <= '9')
      {
         c = std::atoi(s.data());
         consumed += c == 0 ? 1 : static_cast<int64_t>(std::log10(c) + 1); ///< get the # of digits
      }
      else
      {
         system::error_log("invalid version component : {0}", s);
         throw std::runtime_error("invalid version component");
      }

      return consumed;
   }

   /// Create the components of the version from a string.
   /// @return the size of characters consumed from the string, -1 if it failed to parse.
   int64_t parse(std::string_view s) noexcept
   {
      system::debug_log("parsing version : {0}", s);
      try
      {
         int64_t consumed = 0;

         ANTLER_CHECK(!s.empty(), "expected a non-empty version string");

         const auto& component = [&](std::string_view s, uint16_t& comp) {
            ANTLER_CHECK(!s.empty(), "expected a non-empty version string");
            return parse_component(comp, s, true);
         };

         const auto& opt_component = [&](std::string_view s, uint16_t& comp) -> uint64_t {
            if (s.size() > 1 && s[0] == '.')
            {
               return component(s.substr(1), comp) + 1;
            }
            return 0;
         };

         const auto& tweak_component = [&](std::string_view s, std::string& comp) -> uint64_t {
            if (s.size() > 1 && s[0] == '-')
            {
               ANTLER_CHECK(s.size() > 2, "expected a non-empty version string");
               comp = s.substr(1);
               return comp.size() + 1;
            }
            return 0;
         };

         const auto& get_component = [&](auto& comp, auto F) {
            std::decay_t<decltype(comp)> ret = comp;
            int64_t                      amt = F(s, ret);
            ANTLER_CHECK(amt >= 0, "failed to parse component");
            s    = amt >= s.size() ? "" : s.substr(amt);
            comp = ret;
            consumed += amt;
         };

         get_component(major_comp, component);
         get_component(minor_comp, opt_component);
         get_component(patch_comp, opt_component);
         get_component(tweak_comp, tweak_component);

         return consumed;
      }
      catch (...)
      {
         return -1;
      }
   }

   /// @return The string this version was built from.
   [[nodiscard]] inline explicit operator std::string() const noexcept { return to_string(); }

   /// compare the string value of this to rhs. Attempt to use semver rules. `- suffixes will be ignored`
   /// @param rhs  The version to compare to.
   /// @return the result of the comparison: eq, lt, gt.
   [[nodiscard]] int64_t compare(const version& o) const noexcept
   {
      const auto& compare_one = [](auto v1, auto v2, int64_t c) -> int64_t {
         if (c == 0)
         {
            if (v1 == v2)
               return 0;
            else if (v1 > v2)
               return 1;
            return -1;
         }
         return c;
      };

      return compare_one(patch(), o.patch(), compare_one(minor(), o.minor(), compare_one(major(), o.major(), 0)));
   }

   /// Get the major component of the version.
   /// @return The major component of the version.
   [[nodiscard]] inline uint16_t major() const noexcept { return major_comp; }

   /// Get the minor component of the version.
   /// @return The major component of the version.
   [[nodiscard]] inline uint16_t minor() const noexcept { return minor_comp; }

   /// Get the patch component of the version.
   /// @return The major component of the version.
   [[nodiscard]] inline uint16_t patch() const noexcept { return patch_comp; }

   /// Get the tweak component of the version.
   /// @return The major component of the version.
   [[nodiscard]] inline std::string_view tweak() const noexcept { return tweak_comp; }

   /// Serialization function from version to yaml node
   [[nodiscard]] inline yaml::node_t to_yaml() const noexcept { return yaml::node_t{to_string()}; }

   /// Deserialization function from yaml node to version
   [[nodiscard]] inline bool from_yaml(const yaml::node_t& n) noexcept
   {
      auto s = n.as<std::string>();
      return parse({s.c_str(), s.size()}) > 0;
   }

private:
   uint16_t    major_comp = 0;
   uint16_t    minor_comp = 0;
   uint16_t    patch_comp = 0;
   std::string tweak_comp;
};

// these don't need to be in global namespace ADL will handle finding the right one
inline std::ostream& operator<<(std::ostream& os, const antler::project::version& o)
{
   os << o.to_string();
   return os;
}

} // namespace antler::project

ANTLER_YAML_CONVERSIONS(antler::project::version);
