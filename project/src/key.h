#ifndef src_key_h
#define src_key_h

#include <string>
#include <string_view>
#include <antler/project/object.h>
#include <vector>

namespace key {


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

//template<typename STRING_TYPE>
//word to_word(STRING_TYPE s);
word to_word(std::string_view s);

std::string to_string(word e);

extern const char* literals[];

} // namespace key


std::ostream& operator<<(std::ostream& os, const key::word& e);

namespace std {
inline std::string to_string(key::word e) { return key::to_string(e); };
} // namespace std


#endif
