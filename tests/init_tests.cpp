/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/manifest.hpp>

#include <catch2/catch.hpp>

#include "common.hpp"

TEST_CASE("Testing init subcommand") {
   using namespace antler::project;

   {
      project proj = {std::filesystem::path("./foo") / antler::project::project::manifest_name, "foo", "v1.0.0"};

      remove_file("./foo");
      REQUIRE(proj.init_dirs(std::filesystem::path("./foo")));

      manifest m;

      m.set(proj.to_yaml());

      m.write("./foo");

      manifest m2{"./foo"};

      project proj2 = m2.to_project();

      CHECK(proj2.name() == "foo");
      CHECK(proj2.version() == version{"v1.0.0"});
   }
}