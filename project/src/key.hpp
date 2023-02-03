#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include <string>
#include <string_view>
#include <vector>

#include <antler/project/object.hpp>


namespace key {

// These are the valid keywords for the `project.yaml` file.
enum class word {
   none,

   apps,
   command,
   depends,
   from,
   hash,                        // Deps: indicates the SHA256 hash for an archive. Valid for github release and archive locations.
   lang,
   libs,
   name,
   options,
   patch,
   project,
   release,                     // Deps: this indicates a github version to download.
   tag,                         // Deps: indicates the github tag to clone.
   tests,
   version,                     // Deps: version is a synonym for release.
};

// template<typename STRING_TYPE>
// word to_word(STRING_TYPE s);

/// Convert a string into a word. `none` is returned for a failure to parse.
/// @param s  The string to parse.
/// @return s as a key word, none if s could not be parsed.
word to_word(std::string_view s);

/// Convert e to a string.
/// @param e  The word to convert to a string.
/// @return String representation of e.
std::string to_string(word e);

/// This is a convinience for the yaml encoder/decoder.
extern const char* literals[];

} // namespace key


std::ostream& operator<<(std::ostream& os, const key::word& e);

namespace std {
inline std::string to_string(key::word e) { return key::to_string(e); };
} // namespace std
