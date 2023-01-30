#ifndef antler_string_split_h
#error "bad inclusion"
#endif

/// @copyright See `LICENSE` in the root directory of this project.


namespace antler {
namespace string {


template<typename STRING_TYPE>
inline std::vector<STRING_TYPE> split(std::string_view s, std::string_view split_chars) {
   std::vector<STRING_TYPE> rv;
   while (!s.empty()) {
      auto r = s.find_first_of(split_chars);
      if (r == std::string_view::npos) {
         rv.push_back(STRING_TYPE(s));
         return rv;
      }
      if (r != 0)
         rv.push_back(STRING_TYPE(s.substr(0, r)));
      s = s.substr(r + 1);
   }
   return rv;
}



} // namespace string
} // namespace antler
