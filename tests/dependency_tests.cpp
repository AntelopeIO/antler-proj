/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/dependency.hpp>

#include <catch2/catch.hpp>

TEST_CASE("Testing dependency validating locations")
{
   using namespace antler::project;

   dependency d = {"larryk85/dune"};

   CHECK(d.name() == "dune");
   CHECK(d.location() == "larryk85/dune");

   CHECK(d.is_valid_location());

   d.location("larryk85/does-not-exist");
   CHECK(d.location() == "larryk85/does-not-exist");

   CHECK(!d.is_valid_location());
}

TEST_CASE("Testing dependency yaml conversions")
{
   using namespace antler::project;

   dependency d = {"larryk85/dune"};

   yaml::node_t n = d.to_yaml();

   CHECK(d.location() == n["location"].as<std::string>());
   CHECK(d.tag() == n["tag"].as<std::string>());

   dependency d2;
   CHECK(d2.from_yaml(n));

   CHECK(d.name() == d2.name());
   CHECK(d.location() == d2.location());
   CHECK(d.tag() == d2.tag());
}
