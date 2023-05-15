/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.hpp>
#include "common.hpp"

#include <catch2/catch.hpp>


TEST_CASE("Testing project")
{
   using namespace antler::project;
   app_t apps[] = {
      {"appa", "C",   "", ""},
      {"appb", "C++", "", ""},
      {"appc", "C++", "", ""},
      {"appd", "C++", "", ""}
   };

   apps[0].upsert_dependency({"https://github.com/larryk85/dune", "foo", "v13.3"});
   apps[0].upsert_dependency({"mikelik/dune", "foo", "v13.4"});
   apps[1].upsert_dependency({"https://github.com/larryk85/fast_math", "", "blah"});
   apps[0].upsert_dependency({"https://github.com/antelopeio/leap", "", "v2.2.2v"});
   apps[1].upsert_dependency({"", "libc"});

   lib_t libs[] = {
      {"libb", "C++", "", ""},
      {"libc", "C",   "", ""},
      {"libd", "C++", "", ""}
   };

   libs[0].upsert_dependency({"https://github.com/larryk85/dune", "foo", "main"});
   libs[0].upsert_dependency({"mikelik/dune2", "foo", "branch"});
   libs[0].upsert_dependency({"https://github.com/larryk85/fast_math", ""
                                                                       "blah"});
   libs[1].upsert_dependency({"https://github.com/antelopeio/leap", "", "v2.2.2v"});

   project proj;
   proj.name("test_proj");
   proj.version(version{1, 3, 4});

   proj.upsert(std::move(apps[0]));
   proj.upsert(std::move(libs[0]));
   proj.upsert(std::move(libs[1]));
   proj.upsert(std::move(libs[2]));
   proj.upsert(std::move(apps[1]));
   proj.upsert(std::move(apps[2]));
   proj.upsert(std::move(apps[3]));

   CHECK(proj.name() == "test_proj");
   CHECK(proj.version() == version{1, 3, 4});
   CHECK(proj.version().to_string() == "1.3.4");

   CHECK(proj.apps().size() == 4);
   CHECK(proj.apps()["appa"].dependencies()["foo"].location() == "mikelik/dune");
   CHECK(proj.apps()["appa"].dependencies()["foo"].tag() == "v13.4");
   CHECK(proj.libs().size() == 3);
   CHECK(proj.libs()["libb"].dependencies()["foo"].location() == "mikelik/dune2");
   CHECK(proj.libs()["libb"].dependencies()["foo"].tag() == "branch");
}


TEST_CASE("Testing project yaml conversion")
{
   using namespace antler::project;

   project proj = create_project();

   auto node = proj.to_yaml();

   project proj2;

   CHECK(proj2.from_yaml(node));



   CHECK(proj.name() == proj2.name());
   CHECK(proj.version() == proj2.version());


   CHECK(proj.apps().size() == proj2.apps().size());
   CHECK(proj.libs().size() == proj2.libs().size());

   for (const auto& [k, app] : proj.apps())
   {
      CHECK(proj2.app_exists(app.name()));
   }

   for (const auto& [k, lib] : proj.libs())
   {
      CHECK(proj2.lib_exists(lib.name()));
   }
}