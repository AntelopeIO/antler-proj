#ifndef antler_project_language_h
#define antler_project_language_h

#include <ostream>
#include <istream>
#include <string_view>

namespace antler {
namespace project {

enum class language {
   none,
   c,
   cpp,
   java,
};

language to_language(std::string_view s);

std::string to_string(language e);

extern const char* language_literals[];

} // namespace project
} // namespace antler

std::ostream& operator<<(std::ostream& os, const antler::project::language& e);
std::istream& operator>>(std::istream& is, antler::project::language& e);

namespace std {
inline std::string to_string(antler::project::language e) { return antler::project::to_string(e); };
} // namespace std



#endif
