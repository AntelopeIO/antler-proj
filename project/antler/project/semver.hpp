#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include <iostream>
#include <string_view>
#include <optional>
#include <array>

#include <antler/project/version_compare.hpp>


namespace antler::project {

/// This class encapsulates a semanantic version.
/// For more information about semantic versioning, see: https://semver.org/
class semver {
public:
   using self = semver;         ///< Alias for self type because sometimes it's nice to know who you are.
   using value_type = unsigned; ///< Alias for the internal type.

   /// Constructor: major.minor.patch-pre_release+build
   /// @param major  The major value.
   /// @param minor  The minor value.
   /// @param patch  The patch value.
   /// @param pre_release  The pre_release value.
   /// @param build  The build value.
   semver(value_type major = 0, value_type minor = 0, value_type patch = 0, std::string_view pre_release = "", std::string_view build = "") noexcept;

   [[nodiscard]] int64_t compare(const semver& rhs) const noexcept;

   /// comparison operator
   /// @param rhs  The right hand side semver to compare against.
   /// @return Follows standard rules.
   [[nodiscard]] inline bool operator<(const semver& rhs) const noexcept { return compare(rhs) == -1; }
   [[nodiscard]] inline bool operator<=(const semver& rhs) const noexcept { return compare(rhs) != 1; }
   [[nodiscard]] inline bool operator>(const semver& rhs) const noexcept { return compare(rhs) == 1; }
   [[nodiscard]] inline bool operator>=(const semver& rhs) const noexcept { return compare(rhs) != -1; }

   /// comparison operator
   /// @param rhs  The right hand side semver to compare against.
   /// @return true if this object and rhs are equivalent.
   [[nodiscard]] inline bool operator==(const semver& rhs) const noexcept { return compare(rhs) == 0;}
   /// comparison operator
   /// @param rhs  The right hand side semver to compare against.
   /// @return true if this object and rhs are different (aka NOT equivalent).
   [[nodiscard]] inline bool operator!=(const semver& rhs) const noexcept { return compare(rhs) != 0; }

   /// Clear the version.
   void clear() noexcept;

   /// Print the semver to a string.
   /// @return The semver as a string.
   [[nodiscard]] std::string string() const noexcept;
   /// Print the semver to a stream.
   /// @param os  The stream to print this semver to.
   void print(std::ostream& os) const noexcept;


   /// Factory function to parse a semver from a given string.
   /// @param s  The string to attempt to parse into a semver.
   /// @return An optional either containing a semver if the parse was successful or empty if s was unparseable.
   [[nodiscard]] static std::optional<self> parse(std::string_view s) noexcept;


private:
   /// compare prerelease according to rule 12. lhs and rhs must both be the pre-release portion of the version and
   /// @TODO Initial development erroneously used 2.0.0-rc.1; this function needs to be reevaluated for correctness and renamed.
   [[nodiscard]] static int64_t compare_p_rule12(std::string_view lhs, std::string_view rhs) noexcept;
   /// compare build according to rule 12. lhs and rhs must both be the build portion of the version.
   /// @TODO Initial development erroneously used 2.0.0-rc.1; this function needs to be reevaluated for correctness and renamed.
   [[nodiscard]] static int64_t compare_b_rule12(std::string_view lhs, std::string_view rhs) noexcept;
   /// @return a comparison of lhs and rhs according to semver rule 12. lhs and rhs must both be EITHER pre-release or build and
   /// both must be populated.
   /// @TODO Initial development erroneously used 2.0.0-rc.1; this function needs to be reevaluated for correctness and renamed.
   [[nodiscard]] static int64_t compare_pb_rule12(std::string_view lhs, std::string_view rhs) noexcept;
   /// @return true indivcates valid pre-release or build string; false otherwise.
   /// @TODO Initial development erroneously used 2.0.0-rc.1; this function needs to be reevaluated for correctness and renamed.
   [[nodiscard]] static bool validate_pb_rule10or11(std::string_view s) noexcept;

private:
   std::array<value_type, 4> m_xyz; ///< Major, minor, and patch
   std::string m_pre;               ///< pre-release string, possibly empty.
   std::string m_build;             /// build number string, possibly empty.
};

} // namespace antler::project


inline std::ostream& operator<<(std::ostream& os, const antler::project::semver& o) { o.print(os); return os; }
