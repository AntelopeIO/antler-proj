#ifndef antler_string_trim_hpp
#error "bad inclusion"
#endif

/// @copyright See `LICENSE` in the root directory of this project.


namespace antler {
namespace string {


inline std::string_view trim(std::string_view s) {
   if (s.empty())
      return s;

   // trim front, first iteration is guaranteed valid.
   size_t i = 0;
   while (std::isspace(s[i])) {
      ++i;
      // Exit if the string is ALL whitespace.
      if (i == s.size())
         return std::string_view{};
   }
   s = s.substr(i);

   // trim rear, first iteration is guaranteed valid.
   i = s.size() - 1;
   while (std::isspace(s[i]))
      --i;

   return s.substr(0, i + 1);
}



} // namespace string
} // namespace antler
