#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include <iostream>
#include <string_view>

#include <antler/project/project.hpp>

inline bool remove_file(std::string_view fn) { return antler::system::fs::remove_all(fn); }

inline bool load_project(std::string_view fn, antler::project::project& proj) {
   using namespace antler::project;
   auto p = antler::system::fs::canonical(antler::system::fs::path(fn));
   if (!project::update_path(p)) {
      return false;
   }

   proj.path(p.parent_path());
   return proj.from_yaml(yaml::load(p));
}

static antler::project::project create_project() {
   using namespace antler::project;

   app_t apps[] = { {"appa", "C", "-M", "-flto"},
                  {"appb", "C++", "-std=c++14;-Mm", "-ld"},
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

   return proj;
}
