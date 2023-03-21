/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/location.hpp>

#include <catch2/catch.hpp>

TEST_CASE("Testing location clone") {
   using namespace antler::project;

   location::clone_github_repo("antelopeio", "antler-proj", "main", 10, "./foo/foo2");

   CHECK(antler::system::fs::exists("./foo/foo2/.git"));

   location::clone_git_repo("https://github.com/antelopeio/antler-proj", "main", 10, "./foo/bar");

   CHECK(antler::system::fs::exists("./foo/bar/.git"));
}

TEST_CASE("Testing location github REST API requests") {
   using namespace antler::project;

   std::string antler_proj_default_branch = location::get_github_default_branch("antelopeio", "antler-proj");
   CHECK(antler_proj_default_branch == "main");

   CHECK_THROWS(location::get_github_default_branch("antelopeio", "repo-does-not-exist"));
}