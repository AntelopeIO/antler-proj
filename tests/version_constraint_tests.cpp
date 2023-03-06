/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/version_constraint.hpp>

#include <catch2/catch.hpp>
#include "common.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using namespace antler::project;

#if 0
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
#endif

inline bool test_constraint(std::string_view v, std::string_view c) { return version_constraint{c}.test(version{v}); }

TEST_CASE("Testing version constraints") {
   REQUIRE(test_constraint({"999"}, {""}));
   REQUIRE(test_constraint({"1.0.0"}, {"1.0.0"}));
   REQUIRE(test_constraint({"1.0.0"}, {"<=1.0.0"}));
   REQUIRE(test_constraint({"1.0.0"}, {">=1.0.0"}));
   REQUIRE(!test_constraint({"1.0.0"}, {"<1.0.0"}));
   REQUIRE(!test_constraint({"1.0.0"}, {">1.0.0"}));
}