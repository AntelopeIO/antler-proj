#pragma once

#include <iostream>
#include <string>

#include <CLI11.hpp>

#include <antler/project/project.hpp>

namespace antler {
   struct init_project {
      inline init_project(CLI::App& app) {
         subcommand = app.add_subcommand("init", "Initialize a new project creating the directory tree and a `project.yaml` file.");
         subcommand->add_option("-p, path", path, "This is the root path to create the project in.")->required();
         subcommand->add_option("-n, project_name", name, "The name of the project.")->required();
         subcommand->add_option("-v, version", version_raw, "The version to store in the project file.");
      }

      int32_t exec() {
         antler::project::project proj = {std::filesystem::path(path) / antler::project::project::manifest_name, name, version_raw};

         if (!proj.init_dirs(path))
            return -1;

         proj.sync();
         return 0;
      }
      
      CLI::App*   subcommand;
      std::string path;
      std::string name;
      std::string version_raw;
   };
} // namespace antler