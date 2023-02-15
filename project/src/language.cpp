/// @copyright See `LICENSE` in the root directory of this project.

#include <algorithm>            // std::transform
#include <antler/project/language.hpp>


namespace antler::project {


language to_language(std::string_view s) {

   // Try string as is.
   auto lang = magic_enum::enum_cast<language>(s);
   if (lang.has_value())
      return lang.value();

   // Then try again after downcasing.
   std::string dc{s};
   std::transform(dc.cbegin(), dc.cend(), dc.begin(), [](unsigned char c) { return std::tolower(c); });

   lang = magic_enum::enum_cast<language>(s);
   if (lang.has_value())
      return lang.value();

   // Unable to find a matching string thus far, we now try some aliases before returning none.
   if (dc == "c++")
      return antler::project::language::cpp;

   // All options are exhausted, return none.
   return antler::project::language::none;
}


std::string to_string(language e) {
   return std::string(magic_enum::enum_name(e));
}


std::string_view to_string_view(language e) {
   return magic_enum::enum_name(e);
}


} // namespace antler::project


std::ostream& operator<<(std::ostream& os, const antler::project::language& e) {
   os << to_string(e);
   return os;
}


std::istream& operator>>(std::istream& is, antler::project::language& e) {

   std::string temp;
   if (is >> temp)
      e = antler::project::to_language(temp);
   else {
      // This might be an exceptional state and so maybe we should throw an exception?
      e = antler::project::language::none;
   }
   return is;
}
