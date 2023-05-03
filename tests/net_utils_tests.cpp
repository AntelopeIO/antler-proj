/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/net_utils.hpp>

#include <catch2/catch.hpp>

using namespace std::literals;

TEST_CASE("shorthand") {
   using namespace antler::project;
   
   CHECK(github::is_shorthand("org/project"sv));
   CHECK(github::is_shorthand("org/a"sv));

   CHECK(github::is_shorthand("https://github.com/org/project"sv) == false);
   CHECK(github::is_shorthand("org/project/"sv) == false);
   CHECK(github::is_shorthand("org/"sv) == false);
   CHECK(github::is_shorthand("org"sv) == false);
   CHECK(github::is_shorthand(""sv) == false);
}
