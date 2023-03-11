/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.hpp>
#include <antler/project/dependency.hpp>

#include <catch2/catch.hpp>

#include "common.hpp"

TEST_CASE("Testing init subcommand") {
   using namespace antler::project;

   {
      project proj = {std::filesystem::path("./foo") / antler::project::project::manifest_name, "foo", "v1.0.0"};

      remove_file("./foo");
      REQUIRE(proj.init_dirs(std::filesystem::path("./foo")));

      proj.sync();

      //REQUIRE(load_project("./foo", proj));

      // should fail if directory exists
      //REQUIRE(!proj.init_dirs(std::filesystem::path("./foo")));
   }
}