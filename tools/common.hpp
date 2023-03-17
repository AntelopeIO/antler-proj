#pragma once

#include <filesystem>

#include "CLI11.hpp"

#include <antler/project/project.hpp>

namespace antler {

   inline project::project load_project(const system::fs::path& path) {
      auto p = system::fs::canonical(system::fs::path(path));
      ANTLER_CHECK(project::project::update_path(p),
         "path either did not exist or no `project.yml` file cound be found.");
      project::project proj;
      ANTLER_CHECK(proj.from_yaml(project::yaml::load(path/project::project::manifest_name)),
         "error while loading project.yml file"); 
      
      proj.path(p);
      return proj;
   }
}