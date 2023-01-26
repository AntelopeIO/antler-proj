#ifndef antler_string_split_h
#define antler_string_split_h

#include <string_view>
#include <vector>

namespace antler {
namespace string {

/// @return a vector of parsed values. Note that empty values are ignored.
template<typename STRING_TYPE=std::string_view>
std::vector<STRING_TYPE> split(std::string_view s, std::string_view split_chars);


} // namespace string
} // namespace antler

#include <antler/string/detail/split.ipp>

#endif
