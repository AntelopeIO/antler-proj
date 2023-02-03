#pragma once
#define antler_string_trim_hpp

/// @copyright See `LICENSE` in the root directory of this project.

#include <string_view>

namespace antler {
namespace string {

/// Trim whitespace from a string_view.
/// @note May return an empty string.
/// @param s  The source string.
/// @return A string view with whitespace trimmed from front and rear.
std::string_view trim(std::string_view s);


} // namespace string
} // namespace antler

#include <antler/string/detail/trim.ipp>
