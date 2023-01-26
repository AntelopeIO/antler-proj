#ifndef antler_project_semver_h
#define antler_project_semver_h

#include <iostream>
#include <string_view>

#include <antler/project/version_compare.h>

#include <optional>
#include <array>


namespace antler {
namespace project {

class semver {
public:
   using self = semver;
   using value_type = unsigned;


   semver(value_type x=0, value_type y=0, value_type z=0, std::string_view pre_release="", std::string_view build="") noexcept;

   // comparison operators
   bool operator==(const self& rhs) const noexcept;
   bool operator!=(const self& rhs) const noexcept;
   bool operator<(const self& rhs) const noexcept;
   bool operator<=(const self& rhs) const noexcept;
   bool operator>(const self& rhs) const noexcept;
   bool operator>=(const self& rhs) const noexcept;


   void clear() noexcept;

   cmp_result compare(const self& rhs) const noexcept;

   std::string string() const noexcept;
   void print(std::ostream& os) const noexcept;


   /// Parse a semver from a given string.
   static std::optional<self> parse(std::string_view s) noexcept;


private:
   /// compare prerelease according to rule 12.
   static cmp_result compare_p_rule12(std::string_view lhs, std::string_view rhs) noexcept;
   /// compare build according to rule 12.
   static cmp_result compare_b_rule12(std::string_view lhs, std::string_view rhs) noexcept;
   /// @return a comparison of lhs and rhs according to semver rule 12. lhs and rhs must both be EITHER pre-release or build and
   /// both must be populated.
   static cmp_result compare_pb_rule12(std::string_view lhs, std::string_view rhs) noexcept;
   /// @return true indivcates valid pre-release or build string; false otherwise.
   static bool validate_pb_rule10or11(std::string_view lhs) noexcept;

private:
   std::array<value_type, 4> m_xyz;
   std::string m_pre;           // pre-release
   std::string m_build;         // build number
};


} // namespace project
} // namespace antler


inline std::ostream& operator<<(std::ostream& os, const antler::project::semver& o) { o.print(os); return os; }


#endif
