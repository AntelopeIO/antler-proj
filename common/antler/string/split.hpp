#ifndef antler_string_split_h
#define antler_string_split_h

/// @copyright See `LICENSE` in the root directory of this project.

#include <string_view>
#include <vector>

namespace antler {
namespace string {

/// Parse a string into substrings.
/// @tparam STRING_TYPE  This parameter defines the value_type of the returned vector. It defaults to `std::string_view`, which is normally quite optimal; however, sometimes it is more optimal to return a vector of `std::string`.
/// @param s  The input to
/// @param split_chars  This is a string of chars used to perform the split. Note that the split chars are not included in the return value.
/// @return a vector of parsed values. Note that empty values are ignored.
template<typename STRING_TYPE = std::string_view>
std::vector<STRING_TYPE> split(std::string_view s, std::string_view split_chars);


} // namespace string
} // namespace antler

#include <antler/string/detail/split.ipp>

#endif
