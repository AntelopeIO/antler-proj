#ifndef	antler_string_from_h
# error "bad inclusion"
#endif



namespace antler {
namespace string {


template<typename T>
inline bool from(std::string_view s, T& n) {
   n = 0;
   for(auto c : s) {
      n *= 10;
      if(c < '0' || c > '9')
         return false;
      n += c-'0';
   }
   return true;
}


// Specialization for ensure a char comes in as a uint8_t.
template<>
inline bool from(std::string_view s, uint8_t& rv) {
  unsigned u;
  if (!from(s,u))
    return false;
  rv = uint8_t(u);
  return true;
}


} // namespace string
} // namespace antler
