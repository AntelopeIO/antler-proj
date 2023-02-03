#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/semver.hpp>

#include <string>
#include <string_view>
#include <optional>
#include <array>
#include <memory>



namespace antler {
namespace project {

/// Simple class to encapsulate a project version.
class version {
public:
   using self = version;        ///< Alias for self type.

   /// Default constructor.
   version();
   /// @parm ver  A string to create this version with. ver is evaluated to see if it might be a semver.
   version(std::string_view ver);
   /// @param sv  A semver version to create this version with.
   version(const semver& sv);
   /// Copy constructor.
   /// @parm rhs  Source to copy from
   version(const self& rhs);

   /// @parm ver  A string to create this version with. ver is evaluated to see if it might be a semver.
   version& operator=(std::string_view ver);
   /// @param sv  A semver version to create this version with.
   version& operator=(const semver& sv);
   /// @parm rhs  Source to copy from
   version& operator=(const self& rhs);

   /// Clear any values.
   void clear() noexcept;

   /// @return true if this version is empty.
   bool empty() const noexcept;
   /// @return The raw string this version was built from. If created from a semver, the string equivalent.
   std::string_view raw() const noexcept;

   /// @return true if this version is a semver.
   bool is_semver() const noexcept;
   /// @return The version in semver_t format. If is_semver() would return false, this value is invalid.
   operator semver() const noexcept;

   // comparison operators:
   bool operator==(const self& rhs) const noexcept;
   bool operator!=(const self& rhs) const noexcept;
   bool operator<(const self& rhs) const noexcept;
   bool operator<=(const self& rhs) const noexcept;
   bool operator>(const self& rhs) const noexcept;
   bool operator>=(const self& rhs) const noexcept;

private:
   /// internal class for comparisons.
   enum class cmp {
      eq,
      lt,
      gt,
   };

   /// compare the string value of this to rhs. Attempt to use semver rules.
   /// @param rhs  The version to compare to.
   /// @return the result of the comparison: eq, lt, gt.
   static cmp raw_compare(std::string_view l_in, std::string_view r_in) noexcept;
   /// Compare this to rhs. Sart by attempting to compare semver values; fall back to raw_compare()
   /// @param rhs  The version to compare to.
   /// @return the result of the comparison: eq, lt, gt.
   cmp compare(const self& rhs) const noexcept;

   /// Load this version from a string. Attempts to parse and store as semver in the process. Either way, s is stored as m_raw.
   /// @param s  The string to store.
   void load(std::string_view s);
   /// Load this version from a semver. sv is stored in m_semver and printed into m_raw.
   void load(const semver& sv);

private:
   std::string m_raw;                ///< The raw, printable value of the version.
   std::unique_ptr<semver> m_semver; ///< If valid, the semver equivalent of m_raw.
};


} // namespace project
} // namespace antler

inline std::ostream& operator<<(std::ostream& os, const antler::project::version& o) { os << o.raw(); return os; }
//std::istream& operator>>(std::istream& is, antler::project::object::version& e);
