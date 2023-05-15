/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/version_constraint.hpp>

#include <catch2/catch.hpp>

#include <string>
#include <vector>
#include <iostream>

using namespace antler::project;

inline bool test_constraint(std::string_view v, std::string_view c)
{
   return version_constraint{c}.test(version{v});
}

TEST_CASE("Testing version constraints parsing")
{
   using namespace antler::project;

   {
      version_constraint vc("v2.2.4");

      auto con = vc.constraints();
      CHECK(con.size() == 1);
      CHECK(con[0].lower_bound.rel == relation::eq);
      CHECK(con[0].lower_bound.ver == version("2.2.4"));
      CHECK(!con[0].upper_bound);
   }

   {
      version_constraint vc(">2.2.3-rc1");

      auto con = vc.constraints();
      CHECK(con.size() == 1);
      CHECK(con[0].lower_bound.rel == relation::gt);
      CHECK(con[0].lower_bound.ver == version("v2.2.3-rc1"));
      CHECK(!con[0].upper_bound);
   }

   {
      version_constraint vc(">v2.2.3-rc1");

      auto con = vc.constraints();
      CHECK(con.size() == 1);
      CHECK(con[0].lower_bound.rel == relation::gt);
      CHECK(con[0].lower_bound.ver == version("v2.2.3-rc1"));
      CHECK(!con[0].upper_bound);
   }

   {
      version_constraint vc(">=2.2.3-rc1");

      auto con = vc.constraints();
      CHECK(con.size() == 1);
      CHECK(con[0].lower_bound.rel == relation::ge);
      CHECK(con[0].lower_bound.ver == version("v2.2.3-rc1"));
      CHECK(!con[0].upper_bound);
   }

   {
      version_constraint vc("<2.2.3-rc1");

      auto con = vc.constraints();
      CHECK(con.size() == 1);
      CHECK(con[0].lower_bound.rel == relation::lt);
      CHECK(con[0].lower_bound.ver == version("v2.2.3-rc1"));
      CHECK(!con[0].upper_bound);
   }

   {
      version_constraint vc("<=2.2.3-rc1");

      auto con = vc.constraints();
      CHECK(con.size() == 1);
      CHECK(con[0].lower_bound.rel == relation::le);
      CHECK(con[0].lower_bound.ver == version("v2.2.3-rc1"));
      CHECK(!con[0].upper_bound);
   }

   {
      CHECK_THROWS(version_constraint("<=d.2.3-rc1"));
      CHECK_THROWS(version_constraint("l1.2.3-rc1"));
      CHECK_THROWS(version_constraint("<=1.2.3*rc1"));
      CHECK_THROWS(version_constraint(">>d.2.3-rc1"));
   }
}

TEST_CASE("Testing version constraints")
{
   REQUIRE(test_constraint("999", {}));
   REQUIRE(test_constraint("1.0.0", {"1.0.0"}));
   REQUIRE(test_constraint("1.0.0", {"<= 1.0.0"}));
   REQUIRE(test_constraint("1.0.0", {">= 1.0.0"}));
   REQUIRE(!test_constraint("1.0.0", {"< 1.0.0"}));
   REQUIRE(!test_constraint("1.0.0", {"> 1.0.0"}));
   REQUIRE(test_constraint("2.1", {"> 2, < 3"}));

   REQUIRE(test_constraint("2.0.12", {">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2"}));
   REQUIRE(test_constraint("2.1.3", {">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2"}));
   REQUIRE(test_constraint("2.2.3", {">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2"}));
   REQUIRE(test_constraint("2.2.99", {">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2"}));
   REQUIRE(test_constraint("2.3.2", {">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2"}));
   REQUIRE(test_constraint("3.2", {">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2"}));
   REQUIRE(test_constraint("3.3", {">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2"}));

   REQUIRE(!test_constraint("2.0", {">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2"}));
   REQUIRE(!test_constraint("2.1", {">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2"}));
   REQUIRE(!test_constraint("2.2", {">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2"}));
   REQUIRE(!test_constraint("2.3", {">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2"}));
   REQUIRE(!test_constraint("3.0", {">= 2.0.12, < 2.1 | >= 2.1.3, < 2.2 | >= 2.2.3, < 2.3 | >= 2.3.2, < 3 | >= 3.2"}));
}
