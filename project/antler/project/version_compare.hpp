#pragma once

/// Support for comparisons.
/// @copyright See `LICENSE` in the root directory of this project.

#include <iostream>
#include <string_view>


namespace antler::project {

/// Enum for comparison results.
enum class cmp_result {
   eq,                          ///< equivalent
   lt,                          ///< less than
   gt,                          ///< greater than
};
void print(std::ostream& os, cmp_result e) noexcept;

/// compare lhs and rhs by splitting on '.' and comparing results. This is a simple compare and does not consider semver or any other precedence.
/// @return `lt` if lhs < rhs; `gt` if lhs > rhs; `eq` if lhs == rhs.
cmp_result raw_compare(std::string_view lhs, std::string_view rhs) noexcept;


} // namespace antler::project


inline std::ostream& operator<<(std::ostream& os, const antler::project::cmp_result& e) { antler::project::print(os,e); return os; }
