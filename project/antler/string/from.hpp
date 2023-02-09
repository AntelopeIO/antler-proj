#pragma once

/// Convert from string into some numberic type.
/// @copyright See `LICENSE` in the root directory of this project.


#include <string_view>
#include <charconv>
#include <sstream>

namespace antler::string {

/// Convert text to a numeric value. Works for int and unsigned of all widths.
/// @warn Does NOT work for floating points for older compilers (e.g. gcc-10)
/// @warn The string must not contain whitespace.
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
[[nodiscard]] inline bool from(std::string_view s, T& rv) noexcept {
   T n=0;
   auto result = std::from_chars(s.begin(), s.end(), n);
   if(result.ptr != s.end())
      return false;
   rv = n;
   return true;
}


// Specialization to ensure a uint8_t is converted correctly.
template<>
[[nodiscard]] inline bool from(std::string_view s, uint8_t& rv) noexcept {
   unsigned u=0;
   auto result = std::from_chars(s.begin(), s.end(), u);
   if(result.ptr != s.end())
      return false;
   if(uint8_t(u) < u)                    // overflow protection.
      return false;
   rv = uint8_t(u);
   return true;
}


} // namespace antler::string
