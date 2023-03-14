#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

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


namespace antler::project {

/// Simple class to encapsulate a project version.
class version {
public:
   using self = version;        ///< Alias for self type.

   /// @param ver  A string to create this version with.
   inline version(std::string_view ver) {
      if (!from_string(ver, major_comp, minor_comp, patch_comp, tweak_comp))
         throw std::runtime_error("version malformed");
   }

   /// @param maj  Major version component.
   /// @param min  Minor version component.
   /// @param pat  Patch version component.
   /// @param tweak  Tweak version component.
   inline version(uint16_t maj=0, uint16_t min=0, uint16_t pat=0, std::string tweak="")
      : major_comp(maj), minor_comp(min), patch_comp(pat), tweak_comp(std::move(tweak)) {}

   /// Copy constructor.
   /// @param rhs  Source to copy from
   version(const self& rhs) = default;

   /// @param ver  A string to create this version with. ver is evaluated to see if it might be a semver.
   inline version& operator=(std::string_view ver) {
      if (!from_string(ver, major_comp, minor_comp, patch_comp, tweak_comp))
         throw std::runtime_error("version malformed");
      return *this;
   }

   /// @param rhs  Source to copy from
   version& operator=(const self& rhs) = default;

   /// comparison operator
   /// @param rhs  The right hand side semver to compare against.
   /// @return Follows standard rules.
   //[[nodiscard]] std::strong_ordering operator<=>(const self& rhs) const;
   [[nodiscard]] inline bool operator<(const self& rhs) const noexcept { return compare(rhs) == -1; }
   [[nodiscard]] inline bool operator<=(const self& rhs) const noexcept{ return compare(rhs) != 1; }
   [[nodiscard]] inline bool operator>(const self& rhs) const noexcept { return compare(rhs) == 1; }
   [[nodiscard]] inline bool operator>=(const self& rhs) const noexcept { return compare(rhs) != -1; }

   // comparison operators:
   [[nodiscard]] inline bool operator==(const self& rhs) const noexcept { return compare(rhs) == 0; }
   [[nodiscard]] inline bool operator!=(const self& rhs) const noexcept { return compare(rhs) != 0; }

   /// Clear any values.
   inline void clear() noexcept {
      major_comp = 0;
      minor_comp = 0;
      patch_comp = 0;
      tweak_comp = "";
   }

   /// @return true if this version is empty.
   [[nodiscard]] inline bool empty() const noexcept {
      return major() == 0 &&
             minor() == 0 &&
             patch() == 0 &&
             tweak().empty();
   }

   /// @return The string this version was built from.
   [[nodiscard]] std::string to_string() const noexcept {
      std::string ret = std::to_string(major_comp)+"."+std::to_string(minor_comp)+"."+std::to_string(patch_comp);
      if (!tweak_comp.empty())
         ret += "-" +tweak_comp;
      return ret;
   }

   [[nodiscard]] static inline bool to_component(uint16_t& c, std::string_view s) {
      try {
         if (s[0] == 'v' || s[0] == 'V')
            s = s.substr(1);
         c = std::atoi(s.data());
      } catch(const std::invalid_argument&) {
         std::cerr << "component :" << s << " not a valid component." << std::endl;
         return false;
      } catch(const std::out_of_range&) {
         std::cerr << "component :" << s << " not a valid component." << std::endl;
         return false;
      }
      return true;
   }

   /// Create the components of the version from a string.
   /// @return false if version string is ill-formed.
   [[nodiscard]] static bool from_string(std::string_view s, uint16_t& maj, uint16_t& min, uint16_t& patch, std::string& tweak) noexcept {
      maj = 0;
      min = 0;
      patch = 0;
      tweak = "";
      auto comps = system::split<'.', '-'>(s);

      bool ret = true;

      switch (comps.size()) {
         case 4:
            tweak = comps[3];
            [[fallthrough]];
         case 3:
            ret = to_component(patch, comps[2]);
            [[fallthrough]];
         case 2:
            ret &= to_component(min, comps[1]);
            [[fallthrough]];
         case 1:
            ret &= to_component(maj, comps[0]);
            return ret;
         default:
            std::cerr << "Version string is malformed " << s << std::endl;
            return false;
      }
   }

   /// Create the components of the version from a string.
   bool from_string(std::string_view s) {
      return from_string(s, major_comp, minor_comp, patch_comp, tweak_comp);
   }

   /// @return The string this version was built from.
   [[nodiscard]] inline explicit operator std::string() const noexcept { return to_string(); }

   /// compare the string value of this to rhs. Attempt to use semver rules.
   /// @param rhs  The version to compare to.
   /// @return the result of the comparison: eq, lt, gt. If rhs is illformed the return is 1;
   [[nodiscard]] int64_t compare(std::string_view rhs) const noexcept {
      version v;
      if (from_string(rhs, v.major_comp, v.minor_comp, v.patch_comp, v.tweak_comp))
         return 1;
      return compare(v);
   }

   /// compare the string value of this to rhs. Attempt to use semver rules. `- suffixes will be ignored`
   /// @param rhs  The version to compare to.
   /// @return the result of the comparison: eq, lt, gt.
   [[nodiscard]] int64_t compare(const version& o) const noexcept {
      const auto& compare_one = [](auto v1, auto v2, int64_t c) -> int64_t {
         if (c == 0) {
            if (v1 == v2)
               return 0;
            else if (v1 > v2)
               return 1;
            else
               return -1;
         }
         return c;
      };

      return compare_one(patch(), o.patch(),
               compare_one(minor(), o.minor(),
                  compare_one(major(), o.major(), 0)));
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
   [[nodiscard]] inline bool from_yaml(const yaml::node_t& n) noexcept {
      std::string s = n.as<std::string>();
      return from_string({s.c_str(), s.size()});
   }

private:
   uint16_t major_comp;
   uint16_t minor_comp;
   uint16_t patch_comp;
   std::string tweak_comp;
   std::string raw_str;
};

} // namespace antler::project

inline std::ostream& operator<<(std::ostream& os, const antler::project::version& o) { os << o.to_string(); return os; }

ANTLER_YAML_CONVERSIONS(antler::project::version);