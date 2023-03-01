#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/semver.hpp>

#include <string>
#include <string_view>
#include <optional>
#include <array>
#include <memory>
#include <compare>


namespace antler::project {

/// Simple class to encapsulate a project version.
class version {
public:
   using self = version;        ///< Alias for self type.

   /// Default constructor.
   version();
   /// @parm ver  A string to create this version with. ver is evaluated to see if it might be a semver.
   version(std::string_view ver);
   /// @param sv  A semver version to create this version with.
   explicit version(const semver& sv);
   /// Copy constructor.
   /// @parm rhs  Source to copy from
   version(const self& rhs);

   /// @parm ver  A string to create this version with. ver is evaluated to see if it might be a semver.
   version& operator=(std::string_view ver);
   /// @param sv  A semver version to create this version with.
   version& operator=(const semver& sv);
   /// @parm rhs  Source to copy from
   version& operator=(const self& rhs);

   /// comparison operator
   /// @param rhs  The right hand side semver to compare against.
   /// @return Follows standard rules.
   //[[nodiscard]] std::strong_ordering operator<=>(const self& rhs) const;
   [[nodiscard]] std::strong_ordering operator<=>(const self& rhs) const noexcept;
   // comparison operators:
   [[nodiscard]] bool operator==(const self& rhs) const noexcept;
   [[nodiscard]] bool operator!=(const self& rhs) const noexcept;

   /// Clear any values.
   void clear() noexcept;

   /// @return true if this version is empty.
   [[nodiscard]] bool empty() const noexcept;
   /// @return The raw string this version was built from. If created from a semver, the string equivalent.
   [[nodiscard]] std::string_view raw() const noexcept;

   /// @return true if this version is a semver.
   [[nodiscard]] bool is_semver() const noexcept;
   /// @return The version in semver_t format. If is_semver() would return false, this value is invalid.
   explicit operator semver() const noexcept;

private:
   /// compare the string value of this to rhs. Attempt to use semver rules.
   /// @param rhs  The version to compare to.
   /// @return the result of the comparison: eq, lt, gt.
   [[nodiscard]] static std::strong_ordering raw_compare(std::string_view l_in, std::string_view r_in) noexcept;

   /// Load this version from a string. Attempts to parse and store as semver in the process. Either way, s is stored as m_raw.
   /// @param s  The string to store.
   void load(std::string_view s);
   /// Load this version from a semver. sv is stored in m_semver and printed into m_raw.
   void load(const semver& sv);

private:
   std::string m_raw;                ///< The raw, printable value of the version.
   std::unique_ptr<semver> m_semver; ///< If valid, the semver equivalent of m_raw.
};


} // namespace antler::project


inline std::ostream& operator<<(std::ostream& os, const antler::project::version& o) { os << o.raw(); return os; }
