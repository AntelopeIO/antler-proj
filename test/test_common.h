#ifndef antelope_test_common_h
#define antelope_test_common_h


namespace global {

int result = 0;

} // namespace global


/// Evaluate X. On failure print STR, X, and increment global result.
/// @param STR  Value to print if the tests fails.
#define TEST(STR, X) {                                                  \
      if( !(X) ) {                                                      \
         std::cerr << __FILE__ << ":" << __LINE__ << " - " << STR << " - error: failed test \"" << #X << "\"\n"; \
         ++global::result;                                              \
      }                                                                 \
   } \
   /* end TEST */


inline int result() {

   return -global::result;

}


#endif
