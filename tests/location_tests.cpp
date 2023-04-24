/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/location.hpp>

#include <catch2/catch.hpp>

using namespace std::literals;
using namespace antler::project;

TEST_CASE("Testing location clone") {
   antler::system::fs::remove_all("./clone_test");

   CHECK(location::clone_github_repo("antelopeio", "antler-proj", "main", 10, "./clone_test/foo2"));
   CHECK(antler::system::fs::exists("./clone_test/foo2/.git"));

   CHECK(location::clone_git_repo("https://github.com/larryk85/cturtle", "main", 10, "./clone_test/foo3"));
   CHECK(antler::system::fs::exists("./clone_test/foo3/.git"));
}

TEST_CASE("Testing location github REST API requests") {
   std::string default_branch = location::get_github_default_branch("antelopeio", "antler-proj");
   CHECK(default_branch == "main");

   default_branch = location::get_github_default_branch("catchorg", "Catch2");
   CHECK(default_branch == "devel");

   CHECK_THROWS(location::get_github_default_branch("antelopeio", "repo-does-not-exist"));
}

TEST_CASE("strip_github_url_to_shorthand") {
   CHECK(location::strip_github_url_to_shorthand("https://github.com/org/project"sv) == "org/project"sv);
   CHECK(location::strip_github_url_to_shorthand("https://github.com/org/project/z"sv) == "org/project/z"sv);
   CHECK(location::strip_github_url_to_shorthand("https://github.com/org"sv) == "org"sv);
   CHECK_THROWS(location::strip_github_url_to_shorthand("https://github.com"sv));
   CHECK_THROWS(location::strip_github_url_to_shorthand("https://github.com/"sv));
   CHECK_THROWS(location::strip_github_url_to_shorthand(""sv));
}

TEST_CASE("normalize") {
   CHECK(location::normalize("https://github.com/org/project"sv) == "org/project"sv);
   CHECK(location::normalize("https://github.com/org/project/z"sv) == "org/project/z"sv);
   CHECK(location::normalize("https://github.com/org"sv) == "org"sv);
   CHECK(location::normalize("https://xyz.com"sv) == "https://xyz.com"sv);
   CHECK(location::normalize(""sv) == ""sv);
   CHECK(location::normalize("https://github.com"sv) == "https://github.com"sv);
   CHECK(location::normalize("https://github.comx"sv) == "https://github.comx"sv);

   CHECK_THROWS(location::normalize("https://github.com/"sv));
}