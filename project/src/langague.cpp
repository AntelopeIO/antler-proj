#include <antler/project/language.h>

#define LANGUAGE_CASE_OF                        \
   CASE_OF(none, "none")                        \
                                                \
   CASE_OF(c,    "c")                           \
   CASE_OF(cpp,  "cpp")                         \
   CASE_OF(java, "java")                        \
   /* end LANGUAGE_CASE_OF */



namespace antler {
namespace project {


const char* language_literals[] {
#define CASE_OF(E,STR) STR,
   LANGUAGE_CASE_OF
#undef CASE_OF
};

language to_language(std::string_view s) {

#define CASE_OF(X,Y) if( s == Y) return antler::project::language::X;
   LANGUAGE_CASE_OF
      ;
#undef CASE_OF

   // This should be changed to have an internal function:
   //   Call it once, if it returns none, downcase `s` and call it again.

   // Some additional values:
   if( s == "C++" || s == "CPP" || s == "c++" )
      return antler::project::language::cpp;
   if( s == "C" )
      return antler::project::language::c;
   if( s == "Java" || s == "JAVA" )
      return antler::project::language::java;

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
   if(is >> temp)
      e = antler::project::to_language(temp);
   else
      // This might be an exceptional state and so maybe we should throw an exception?
      e = antler::project::language::none;
   return is;
}
