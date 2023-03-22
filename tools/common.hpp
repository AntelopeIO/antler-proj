#pragma once

#include "CLI11.hpp"

#include <antler/project/project.hpp>
#include <antler/project/cmake.hpp>

namespace antler {

   inline project::project load_project(const system::fs::path& path) {
      auto p = system::fs::canonical(system::fs::path(path));
      ANTLER_CHECK(project::project::update_path(p),
         "path either did not exist or no `project.yml` file cound be found.");
      project::project proj;
      ANTLER_CHECK(proj.from_yaml(project::yaml::load(p)),
         "error while loading project.yml file"); 
      proj.path(p.parent_path());
      return proj;
   }

   bool should_repopulate(project::project& proj) {
      auto p = proj.path() / project::project::manifest_name;
      auto build = proj.path() / "build" / project::cmake_lists::filename;

      auto last_manifest_time = system::fs::last_write_time(p);

      if (!system::fs::exists(build)) {
         return true;
      }

      auto last_pop_time      = system::fs::last_write_time(build);

      return last_pop_time < last_manifest_time;
   }
}