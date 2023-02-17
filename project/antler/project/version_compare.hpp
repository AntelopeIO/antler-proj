#pragma once

/// Support for comparisons.
/// @copyright See `LICENSE` in the root directory of this project.

#include <iostream>
#include <string_view>
#include <compare>

namespace antler::project {

/// compare lhs and rhs by splitting on '.' and comparing results. This is a simple compare and does not consider semver or any other precedence.
/// @return Follows standard comparison.
std::strong_ordering raw_compare(std::string_view lhs, std::string_view rhs) noexcept;


} // namespace antler::project
