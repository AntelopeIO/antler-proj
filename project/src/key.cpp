/// @copyright See `LICENSE` in the root directory of this project.

#include <key.hpp>
#include <ostream>

namespace key {


std::string to_string(word e) {
   return std::string(magic_enum::enum_name(e));
}


std::string_view to_string_view(word e) {
   return magic_enum::enum_name(e);
}


word to_word(std::string_view s) {
   // Try string as is.
   auto opt = magic_enum::enum_cast<word>(s);
   if (opt.has_value())
      return opt.value();

   return key::word::none;
}


} // namespace key


std::ostream& operator<<(std::ostream& os, const key::word& e) {
   os << magic_enum::enum_name(e);
   return os;
}
