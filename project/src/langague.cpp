/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/language.hpp>

// Mapping of enum to string, please maintain string as lowercase.
#define LANGUAGE_CASE_OF                        \
   CASE_OF(none, "none")                        \
                                                \
   CASE_OF(c,    "c")                           \
   CASE_OF(cpp,  "cpp")                         \
   CASE_OF(java, "java")                        \
   /* end LANGUAGE_CASE_OF */



namespace antler {
namespace project {


const char* language_literals[]{
#define CASE_OF(E, STR) STR,
   LANGUAGE_CASE_OF
#undef CASE_OF
};


language to_language(std::string_view s) {

   // Look for an exact match with the lowercase string.
#define CASE_OF(E,STR) if( s == STR) return antler::project::language::E;
   LANGUAGE_CASE_OF
      ;
#undef CASE_OF

   // Unable to find exact match, downcase the input and try again.
   std::string dc{s};
   std::transform(dc.cbegin(), dc.cend(), dc.begin(), [](unsigned char c) { return std::tolower(c); });
#define CASE_OF(E,STR) if( dc == STR) return antler::project::language::E;
   LANGUAGE_CASE_OF
      ;
#undef CASE_OF

   // We have been unable to find a matching string thus far. Try some aliases before returning none.
   if (s == "c++")
      return antler::project::language::cpp;

   // All options are exhausted, return none.
   return antler::project::language::none;
}


std::string to_string(language e) {

   switch(e) {
#define CASE_OF(X,Y) case antler::project::language::X: return Y;
      LANGUAGE_CASE_OF;
#undef CASE_OF
   }

   std::string s = "Unknown antler::project::language (";
   s += std::to_string(unsigned(e));
   s += ")";
   return s;
}


} // namespace project
} // namespace antler


std::ostream& operator<<(std::ostream& os, const antler::project::language& e) {
   switch(e) {
#define CASE_OF(X,Y) case antler::project::language::X: { os << Y;  return os; }
      LANGUAGE_CASE_OF;
#undef CASE_OF
   }
   os << "Unknown antler::project::language (" << unsigned(e) << ")";
   return os;
}


std::istream& operator>>(std::istream& is, antler::project::language& e) {

   std::string temp;
   if (is >> temp)
      e = antler::project::to_language(temp);
   else
      // This might be an exceptional state and so maybe we should throw an exception?
      e = antler::project::language::none;
   return is;
}
