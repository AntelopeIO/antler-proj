#pragma once

/// @copyright See `LICENSE` in the root directory of this project.


#include <ostream>
#include <istream>
#include <string_view>
#include <magic_enum.hpp>


namespace antler::project {

/// enum class to decode and store the language type.
enum class language {
   none,
   c,
   cpp,
   java,
};

/// Convert a string to a language enum value.
/// @param s  The string to evaluate.
/// @return The language equivalent from the string. Will be none on failure to decode.
[[nodiscard]] language to_language(std::string_view s);

/// Convert a language enum into a string.
/// @param e  The enum to print and return.
/// @return The string repersentation of e.
[[nodiscard]] std::string to_string(language e);

/// Convert a language enum into a string.
/// @param e  The enum to print and return.
/// @return The string repersentation of e.
[[nodiscard]] std::string_view to_string_view(language e);

/// This is a convinience for the yaml encoder/decoder.
/// array<string_view, N>
constexpr auto language_literals = magic_enum::enum_names<language>();


} // namespace antler::project


std::ostream& operator<<(std::ostream& os, const antler::project::language& e);
std::istream& operator>>(std::istream& is, antler::project::language& e);

namespace std {
[[nodiscard]] inline std::string to_string(antler::project::language e) { return antler::project::to_string(e); };
} // namespace std
