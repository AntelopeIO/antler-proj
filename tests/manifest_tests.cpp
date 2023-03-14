/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.hpp>
#include <antler/project/manifest.hpp>

#include <catch2/catch.hpp>

#include "common.hpp"


TEST_CASE("Testing manifest") {
   using namespace antler::project;


   project proj = create_project();

   manifest m;
   m.set(YAML::Node{proj});

   manifest m2 = {"."};

   project proj2 = m2.to_project();
}