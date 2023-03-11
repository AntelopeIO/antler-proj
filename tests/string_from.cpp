#include "test_common.hpp"
#include <antler/string/from.hpp>
#include <cmath>
#include <limits>

#pragma GCC diagnostic ignored "-Wfloat-equal"

// This is a quick hack to compare two floats.
template<typename L, typename R>
bool same(const L& l, const R& r) {
   static_assert( sizeof(l) == sizeof(R), "size mismatch");
   return (l == r) || (std::fabs(l - r) <= (std::numeric_limits<L>::epsilon() * std::max( fabs(l), fabs(r) ) ));
}


using namespace antler::string;

TEST_CASE("Testing antler::string")

int main(int,char**) {

   uint8_t u8;
   unsigned u=0;
   int i=0;

   TEST( "basic unsigned", from("8",u) && u == 8 );
   TEST( "overflow uint8_t", !from("999",u8) && u == 8);
   TEST( "basic integer", from("-999",i) && i == -999);
   TEST( "negative number into unsigned", !from("-9",u) && u == 8 );
   TEST( "unsigned", !from("9,000",u) && u == 8 );

   /*
   float f=0.0;
   TEST( "basic float", from("23.2",f) && same(f, 23.2f));
   TEST( "basic float", from("23.2",f) && same(f, 23.2f));
   TEST( "basic float", from("-.23",f) && same(f, -0.23f));
   TEST( "basic float", from("-0.19",f) && same(f, -0.19f));
   TEST( "basic double", from("0.0000000000201",d) && same(d, 0.0000000000201));
   */

   return result();
}
