/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/version_constraint.h>
#include <test_common.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>


struct constraint_entry {
   std::string ver;        // left comparison
   std::string constraint; // right comparison
   bool result;            // expected result of testing if constraint contains ver.
};

const std::vector<constraint_entry> compare_list = {
   {"999",      "",                                                                              true },
   { "1.0.0",   "1.0.0",                                                                         true },
   { "1.0.0",   "<= 1.0.0",                                                                      true },
   { "1.0.0",   ">= 1.0.0",                                                                      true },
   { "1.0.0",   "< 1.0.0",                                                                       false},
   { "1.0.0",   "> 1.0.0",                                                                       false},
   { "2.1",     "> 2, < 3",                                                                      true },

   { "2.0.12",  ">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2", true },
   { "2.1.3",   ">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2", true },
   { "2.2.3",   ">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2", true },
   { "2.2.99",  ">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2", true },
   { "2.3.2",   ">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2", true },
   { "3.2",     ">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2", true },
   { "3.3",     ">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2", true },

   { "2.0",     ">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2", false},
   { "2.1",     ">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2", false},
   { "2.2",     ">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2", false},
   { "2.3",     ">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2", false},
   { "3.0",     ">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2", false},
   { "3.2-rc0", ">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2", false},
};


void test_version() {

   for (auto& a : compare_list) {
      antler::project::version ver(a.ver);
      antler::project::version_constraint cons(a.constraint);
      std::stringstream ss;
      ss << a.ver << " " << a.constraint << " expect " << (a.result ? "true" : "false");
      auto msg = ss.str();

      TEST(msg, cons.test(ver) == a.result)
   }
}



/*
struct constraint_entry {
   version v;
   contstraint c;
   bool e;                      // expectation: constraint wor
   //expectation e;
};



std::vector<std::string> versions {
   { "1.0rc1", ">=1.0.0", false


*/


int main(int, char**) {

   test_version();

   return result();
}
