#pragma once

#include <iostream>
#include <string>

#include "CLI11.hpp"

#include <antler/project/project.hpp>

#include "common.hpp"

namespace antler {
   struct validate_project {
      inline validate_project(CLI::App& app) {
         subcommand = app.add_subcommand("validate", "Validate a project.");
         subcommand->add_option("-p, path", path, "This is the path to the root of the project.")->default_val(".");
         subcommand->add_flag("-V, --verbose", verbose, "Verbose output.");

      }

      int32_t exec() {
         try {
            auto proj = load_project(path);
            if (verbose) {
               std::cout << proj.to_yaml() << std::endl;
            } else {
               system::info_log("Valid format for an antler-proj project.yml");
            }

            if (!proj.has_valid_dependencies()) {
               system::error_log("The project contains invalid dependencies");
               return -1;
            }

            system::info_log("Valid project dependencies and all are reachable");
            return 0;
         } catch(...) {
            system::error_log("Path {0} does not exist", path);
            return -1;
         }
      }
      
      CLI::App*   subcommand = nullptr;
      std::string path       = "";
      bool        verbose    = false;
   };
} // namespace antler