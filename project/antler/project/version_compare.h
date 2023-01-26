#ifndef antler_project_version_compare_h
#define antler_project_version_compare_h

#include <iostream>
#include <string_view>


namespace antler {
namespace project {


enum class cmp_result {
   eq,
   lt,
   gt,
};
void print(std::ostream& os, cmp_result e) noexcept;

/// compare lhs and rhs by splitting on '.' and comparing results. This is a simple compare and does not consider semver presidence.
/// @return `lt` if lhs < rhs; `gt` if lhs > rhs; `eq` if lhs == rhs.
cmp_result raw_compare(std::string_view lhs, std::string_view rhs) noexcept;


} // namespace project
} // namespace antler

inline std::ostream& operator<<(std::ostream& os, const antler::project::cmp_result& e) { antler::project::print(os,e); return os; }

#endif
