/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.hpp>
#include "common.hpp"

#include <catch2/catch.hpp>


TEST_CASE("Testing project") {
   using namespace antler::project;
   app_t apps[] = { {"appa", "C", "", ""},
                  {"appb", "C++", "", ""},
                  {"appc", "C++", "", ""},
                  {"appd", "C++", "", ""} };

   apps[0].upsert_dependency({"foo", "https://github.com/larryk85/dune", "v13.3"});
   apps[1].upsert_dependency({"bar", "https://github.com/larryk85/fast_math", "blah"});
   apps[0].upsert_dependency({"baz", "https://github.com/antelopeio/leap", "v2.2.2v"});
   apps[1].upsert_dependency({"libc", ""});

   lib_t libs[] = { {"libb", "C++", "", ""},
                  {"libc", "C", "", ""},
                  {"libd", "C++", "", ""} };

   libs[0].upsert_dependency({"foo", "https://github.com/larryk85/dune", "v13.3"});
   libs[0].upsert_dependency({"bar", "https://github.com/larryk85/fast_math", "blah"});
   libs[1].upsert_dependency({"baz", "https://github.com/antelopeio/leap", "v2.2.2v"});

   project proj;
   proj.name("test_proj");
   proj.version({1, 3, 4});

   proj.upsert(std::move(apps[0]));
   proj.upsert(std::move(libs[0]));
   proj.upsert(std::move(libs[1]));
   proj.upsert(std::move(libs[2]));
   proj.upsert(std::move(apps[1]));
   proj.upsert(std::move(apps[2]));
   proj.upsert(std::move(apps[3]));

   REQUIRE(proj.name() == "test_proj");
   REQUIRE(proj.version() == version{1, 3, 4});
   REQUIRE(proj.version().to_string() == "1.3.4");

   REQUIRE(proj.apps().size() == 4);
   REQUIRE(proj.libs().size() == 3);
}


TEST_CASE("Testing project yaml conversion") {
   using namespace antler::project;

   project proj = create_project();

   
   std::string psv = "project: test_proj\n"
                          "version: 1.3.4";

   auto nn = YAML::Load(psv);

   std::cout << "NN " << nn << std::endl;

   YAML::Node node;
   node = proj;

   //project proj2 = node["test"].as<project>();



   //REQUIRE(proj.name() == proj2.name());
   //REQUIRE(proj.version() == proj2.version());
   //REQUIRE(proj.apps().size() == proj2.apps().size());
   //REQUIRE(proj.libs().size() == proj2.libs().size());
}