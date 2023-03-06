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
         subcommand->add_option("-p, path", path, "This is the path to the root of the project.")->required();
         subcommand->add_flag("-V, --verbose", verbose, "Verbose output.");

      }

      int32_t exec() {
         try {
            auto proj = load_project(path);
            if (!proj) {
               std::cerr << "Error validating project." << std::endl;
               return -1;
            }
            if (verbose) {
               std::cout << proj->to_yaml() << std::endl;
            } else {
               std::cout << "Valid format for an antler-proj project.yml." << std::endl;
            }

            if (!proj->has_valid_dependencies()) {
               std::cerr << "Project contains invalid dependencies." << std::endl;
               return -1;
            }

            std::cout << "Valid project dependencies and all are reachable." << std::endl;
            return 0;
         } catch(...) {
            std::cerr << "Path: " << path << " not found." << std::endl;
            return -1;
         }
      }
      
      CLI::App*   subcommand;
      std::string path;
      bool        verbose;
   };
} // namespace antler