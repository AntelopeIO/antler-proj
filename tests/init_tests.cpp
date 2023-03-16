/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.hpp>

#include <catch2/catch.hpp>

#include "common.hpp"

TEST_CASE("Testing init subcommand") {
   using namespace antler::project;
   
   project proj = {"./foo", "foo", "v1.0.0"};

   remove_file("./foo");
   REQUIRE(proj.init_dirs("./foo"));

   proj.sync();


   project proj2{"./foo"};

   CHECK(proj2.name() == "foo");
   CHECK(proj2.version() == version{"v1.0.0"});

   CHECK(std::filesystem::exists("./foo"));
   CHECK(std::filesystem::exists("./foo/apps"));
   CHECK(std::filesystem::exists("./foo/libs"));
   CHECK(std::filesystem::exists("./foo/include"));
   CHECK(std::filesystem::exists("./foo/ricardian"));
}