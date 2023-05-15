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
      subcommand->footer(std::string(R"(Examples:)") + "\n\t" + app.get_name() + R"( validate)");
      subcommand->add_option("-p, --path", path, "Path containing the project's yaml file.")->default_val(".");
      subcommand->add_flag("-V, --verbose", verbose, "Verbose output.");
   }

   int32_t exec() {
      auto proj = load_project(path);
      if (verbose) {
         std::cout << proj.to_yaml() << std::endl;
      } else {
         system::info_log("Valid format for an antler-proj project.yaml");
      }

      if (!proj.has_valid_dependencies()) {
         system::error_log("The project contains invalid dependencies");
         return EXIT_FAILURE;
      }

      system::info_log("Valid project dependencies and all are reachable");
      return 0;
   }

   CLI::App*   subcommand = nullptr;
   std::string path;
   bool        verbose = false;
};
} // namespace antler
