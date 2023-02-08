/// @copyright See `LICENSE` in the root directory of this project.


namespace antler::string {


template<typename T>
inline bool from(std::string_view s, T& rv) noexcept {
   T n = 0;
   for (auto c : s) {
      n *= 10;
      if (c < '0' || c > '9')
         return false;
      n += c - '0';
   }
   rv = n;
   return true;
}


// Specialization to ensure a char comes in as a uint8_t.
template<>
inline bool from(std::string_view s, uint8_t& rv) noexcept {
   unsigned u;
   if (!from(s, u))
      return false;
   rv = uint8_t(u);
   return true;
}


} // namespace antler::string
