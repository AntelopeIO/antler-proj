#pragma once

#include <iostream>
#include <string>
#include <sysexits.h>

#include "CLI11.hpp"

#include <antler/project/project.hpp>

namespace antler
{
struct init_project
{
   inline init_project(CLI::App& app)
   {
      subcommand = app.add_subcommand("init", "Initialize a new project creating the directory tree and a `project.yaml` file.");
      subcommand->footer(std::string(R"(Examples:)") + "\n\t" + app.get_name() + R"( init --name MyProjectName --version 1.0.0)");
      subcommand->add_option("-p, --path", path, "The root path to create the project in.");
      subcommand->add_option("-n, --name", name, "The name of the project.")->required();
      subcommand->add_option("-v, --version", version, "The version to store in the project file.")->default_val("0.0.0");
   }

   int32_t exec()
   {
      if (path.empty())
      {
         path = "./" + name;
      }
      antler::project::project proj = {path, name, version};

      if (!proj.init_dirs(path))
         return EX_CANTCREAT;

      proj.sync();

      system::info_log("Created project {0} at location {1}", name, path);
      return 0;
   }

   CLI::App*   subcommand;
   std::string path;
   std::string name;
   std::string version;
};
} // namespace antler
