#pragma once

#include <iostream>
#include <string>

#include "CLI11.hpp"

#include <antler/project/project.hpp>

#include "common.hpp"

namespace antler {
   struct populate_project {
      inline populate_project(CLI::App& app) {
         subcommand = app.add_subcommand("populate", "Populate a project's dependencies and CMake.");
         subcommand->add_option("-p, path", path, "This is the path to the root of the project.")->default_val(".");
         subcommand->add_flag("-f, --force", force, "This flag will force a repopulation of the project.")->default_val(false);
      }

      int32_t exec() {
         auto proj = load_project(path);
 
         if (force || should_repopulate(proj)) {
            system::fs::remove_all(proj.path()/"build");
            ANTLER_CHECK(project::populators::get(proj).populate(), "failed to populate dependencies");
         }

         system::info_log("Project population was successful!");
         return 0;
      }
      
      CLI::App*   subcommand = nullptr;
      std::string path = "";
      bool        force = false;
   };
} // namespace antler