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
         subcommand->add_option("-p, path", path, "This is the path to the root of the project.")->required();
      }

      int32_t exec() {
         auto proj = load_project(path);
         if (!proj) {
            return -1;
         }

         return proj->populate() ? 0 : -1;
      }
      
      CLI::App*   subcommand;
      std::string path;
      bool verbose;
   };
} // namespace antler