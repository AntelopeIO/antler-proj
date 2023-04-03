/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/dependency.hpp>

#include <catch2/catch.hpp>

TEST_CASE("Testing dependency validating locations") {
   using namespace antler::project;

   dependency d = {"test2", "larryk85/dune", "1.0.0"};

   CHECK(d.name() == "test2");
   CHECK(d.location() == "larryk85/dune");

   CHECK(d.is_valid_location());

   d.location("larryk85/does-not-exist");
   CHECK(d.location() == "larryk85/does-not-exist");

   CHECK(!d.is_valid_location());

   // TODO: reinstate these tests when support for general git repos and archives is added
   //d.location("https://github.com/larryk85/cturtle");
   //CHECK(d.is_valid_location());

   //d.location("https://github.com/larryk85/does-not-exist");
   //CHECK(!d.is_valid_location());
}

TEST_CASE("Testing dependency yaml conversions") {
   using namespace antler::project;

   dependency d = {"test", "larryk85/dune", "v1.0.0"};

   yaml::node_t n = d.to_yaml();

   CHECK(d.location() == n["location"].as<std::string>());
   CHECK(d.tag() == n["tag"].as<std::string>());

   dependency d2;
   CHECK(d2.from_yaml(n));

   CHECK(d.name() == d2.name());
   CHECK(d.location() == d2.location());
   CHECK(d.tag() == d2.tag());
}