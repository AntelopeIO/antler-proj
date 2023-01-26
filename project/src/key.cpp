#include <key.h>
#include <ostream>

#define WORD_CASE_OF                               \
   CASE_OF( none,    "none" )                      \
                                                   \
   CASE_OF( apps,    "apps" )                      \
   CASE_OF( command, "command" )                   \
   CASE_OF( depends, "depends" )                   \
   CASE_OF( from,    "from" )                      \
   CASE_OF( hash,    "hash" )                      \
   CASE_OF( lang,    "lang" )                      \
   CASE_OF( libs,    "libraries" )                 \
   CASE_OF( name,    "name" )                      \
   CASE_OF( options, "options" )                   \
   CASE_OF( patch,   "patch" )                     \
   CASE_OF( project, "project" )                   \
   CASE_OF( release, "release" )                   \
   CASE_OF( tag,     "tag" )                       \
   CASE_OF( tests,   "tests" )                     \
   CASE_OF( version, "version" )                   \
   /* end WORD_CASE_OF */



namespace key {


const char* literals[] = {
#define CASE_OF(E, STR) STR,
   WORD_CASE_OF
#undef CASE_OF
};


std::string to_string(word e) {

   switch(e) {
#define CASE_OF(E,STR) case key::word::E: return STR;
      WORD_CASE_OF
         ;
#undef CASE_OF
   }

   std::string s = "Unknown key::word (";
   s += std::to_string(unsigned(e));
   s += ")";
   return s;
}


// template<typename STRING_TYPE>
// word to_word(STRING_TYPE s) {
word to_word(std::string_view s) {

#define CASE_OF(E, STR) \
   if (s == STR)        \
      return key::word::E;
   WORD_CASE_OF;
#undef CASE_OF

   return key::word::none;
}



} // namespace key


std::ostream& operator<<(std::ostream& os, const key::word& e) {

   switch(e) {
#define CASE_OF(E,STR) case key::word::E: os << STR; return os;
      WORD_CASE_OF
         ;
#undef CASE_OF
   }

   os << "Unknown key::word (" << unsigned(e) << ")";
   return os;
}


#undef WORD_CASE_OF
