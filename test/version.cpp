/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/version.h>
#include <test_common.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

enum cmp_result {
   eq,
   lt,
   gt,
};


struct compare_entry {
   std::string l;     // left comparison
   std::string r;     // right comparison
   cmp_result expect; // expectation gt, eq, lt.  Test can then check gt, gte, lt, lte, neq
};

const std::vector<compare_entry> compare_list = {
   {"1.0.0",            "1.0.0",           eq},
   { "1.0.0",           "1.0",             eq},
   { "1.0.0",           "1",               eq},
   { "1.0",             "1.0",             eq},
   { "1.0",             "1",               eq},
   { "1",               "1",               eq},
   { "2.0.0",           "1.0.0",           gt},
   { "1.1.2a",          "1.1.2b",          lt},
   { "1.1.3a",          "1.1.2b",          gt},
   { "1.2.cat",         "1.2.dog",         lt},
   { "1.0 alpha",       "1.0 bravo",       lt},
   { "1.1 alpha",       "1.0 bravo",       gt},
   { "1.1 charly",      "1.1 bravo",       gt},
   { "2.1.3",           "1.2.3",           gt},
   { "2.2.3",           "2.2.3a",          lt},
   { "2.2.33",          "2.2.3a",          gt},
   { "2.2.33",          "2.2.3a",          gt},
   { "1.0.0",           "1.0.0-rc1",       gt},
   { "1.0.0+build.23",  "1.0.0+build.24",  lt},
   { "1.0.0+alpha",     "1.0.0",           gt},
   { "1.0.0-rc1+alpha", "1.0.0-rc2",       lt},
   { "1.0.0-rc1+alpha", "1.0.0-rc2+bravo", lt},
   { "1.0.0+3.7.1",     "1.0.0+3.6.1",     gt},
   { "1.0.0+3.7.1",     "1.0.0+3.99.1",    lt},
};


void test_version() {

   for (auto& a : compare_list) {
      antler::project::version l(a.l);
      antler::project::version r(a.r);
      std::stringstream ss;
      ss << l << " , " << r;
      auto msg = ss.str();

      switch (a.expect) {
         case eq: {
            TEST(msg, l == r);
            TEST(msg, l <= r);
            TEST(msg, l >= r);
            TEST(msg, !(l != r));
            TEST(msg, !(l < r));
            TEST(msg, !(l > r));
            TEST(msg, r == l);
            TEST(msg, r <= l);
            TEST(msg, r >= l);
            TEST(msg, !(r != l));
            TEST(msg, !(r < l));
            TEST(msg, !(r > l));
         } break;
         case lt: {
            TEST(msg, !(l == r));
            TEST(msg, l <= r);
            TEST(msg, !(l >= r));
            TEST(msg, l != r);
            TEST(msg, l < r);
            TEST(msg, !(l > r));
            TEST(msg, !(r == l));
            TEST(msg, !(r <= l));
            TEST(msg, r >= l);
            TEST(msg, r != l);
            TEST(msg, !(r < l));
            TEST(msg, r > l);
         } break;
         case gt: {
            TEST(msg, !(l == r));
            TEST(msg, !(l <= r));
            TEST(msg, l >= r);
            TEST(msg, l != r);
            TEST(msg, !(l < r));
            TEST(msg, l > r);
            TEST(msg, !(r == l));
            TEST(msg, r <= l);
            TEST(msg, !(r >= l));
            TEST(msg, r != l);
            TEST(msg, r < l);
            TEST(msg, !(r > l));
         } break;
      }
   }
}


int main(int, char**) {

   test_version();

   return result();
}
