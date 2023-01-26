#ifndef antler_string_from_h
#define antler_string_from_h

/*
   Sourced from libsb
*/


#include <string_view>

namespace antler {
namespace string {

/// Convert text to a numeric value. Works for int and unsigned of all widths.
/// @warn Does NOT work for negative numbers.
/// @warn The string must contain ONLY valid digits from 0 to 9.
/// @warn Does not work for hex, octal, or binary numbers.
/// @example
///   uint64_t num;
///   if(!antler::string::from<uint64_t>("25",num) {
///     /* do error */
///   }
///   else {
///     std::cout << " Received value: " << num << "\n";
///   }
/// @param s  The text source to convert.
/// @param rv  This is a return value, it's updated if s was convertable to a T.
/// @return  Returns true if rv was updated, false otherwise.
template<typename T>
bool from(std::string_view s, T& rv);

} // namespace string
} // namespace antler

#include <antler/string/detail/from.ipp>

#endif
