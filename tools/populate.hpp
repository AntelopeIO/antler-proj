#pragma once

#include <iostream>
#include <string>

#include "CLI11.hpp"

#include <antler/project/project.hpp>

#include "common.hpp"

namespace antler {
   struct populate_project {
      inline populate_project(CLI::App& app) {
         path = system::fs::current_path().string();
         subcommand = app.add_subcommand("populate", "Populate a project's dependencies and CMake.");
         subcommand->footer(std::string(R"(Examples:)")
               + "\n\t" + app.get_name() +R"( populate ./path-to-project)");
         subcommand->add_option("-p, --path", path, "Path containing the project's yaml file.")->default_val(".");
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
      std::string path;
      bool        force = false;
   };
} // namespace antler
