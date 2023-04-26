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

TEST_CASE("strip_github_com") {
   CHECK(location::strip_github_com("https://github.com/org/project"sv) == "org/project"sv);
   CHECK(location::strip_github_com("https://github.com/org/project/z"sv) == "org/project/z"sv);
   CHECK(location::strip_github_com("https://github.com/org"sv) == "org"sv);
   CHECK(location::strip_github_com("https://xyz.com"sv) == "https://xyz.com"sv);
   CHECK(location::strip_github_com(""sv) == ""sv);
   CHECK(location::strip_github_com("https://github.com"sv) == "https://github.com"sv);
   CHECK(location::strip_github_com("https://github.comx"sv) == "https://github.comx"sv);
   CHECK(location::strip_github_com("https://github.com/"sv) == ""sv);
}