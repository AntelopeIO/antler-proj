/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/location.hpp>

#include <catch2/catch.hpp>

TEST_CASE("Testing location clone") {
   using namespace antler::project;

   antler::system::fs::remove_all("./foo");

   CHECK(location::clone_github_repo("antelopeio", "antler-proj", "main", 10, "./foo/foo2"));
   CHECK(antler::system::fs::exists("./foo/foo2/.git"));

   CHECK(location::clone_git_repo("https://github.com/larryk85/cturtle", "main", 10, "./foo/foo3"));
   CHECK(antler::system::fs::exists("./foo/foo3/.git"));
}

TEST_CASE("Testing location github REST API requests") {
   using namespace antler::project;

   std::string default_branch = location::get_github_default_branch("antelopeio", "antler-proj");
   CHECK(default_branch == "main");

   default_branch = location::get_github_default_branch("catchorg", "Catch2");
   CHECK(default_branch == "devel");

   CHECK_THROWS(location::get_github_default_branch("antelopeio", "repo-does-not-exist"));
}
