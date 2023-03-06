#pragma once

#include <iostream>
#include <string>

#include "CLI11.hpp"

#include <antler/project/project.hpp>
#include <antler/project/cmake.hpp>

#include "common.hpp"

namespace antler {

   struct build_project {
      inline build_project(CLI::App& app) {
         subcommand = app.add_subcommand("build", "Build a project.");
         subcommand->add_option("-p, path", path, "This is the path to the root of the project.")->required();
      }

      bool should_repopulate() {
         auto proj = std::filesystem::path(path) / std::filesystem::path(project::project::manifest_name);
         auto build = std::filesystem::path(path) / std::filesystem::path("build") / std::filesystem::path(project::cmake::cmake_lists);

         auto last_manifest_time = std::filesystem::last_write_time(proj);

         if (!std::filesystem::exists(build)) {
            return true;
         }

         auto last_pop_time      = std::filesystem::last_write_time(build);

         return last_pop_time < last_manifest_time;
      }

      int32_t configure() noexcept {
         auto build_dir = std::filesystem::path(path) / std::filesystem::path("build");
         auto bin_dir = build_dir / std::filesystem::path("antler-bin");

         std::filesystem::create_directory(bin_dir);
         std::string cmake_cmd = "cmake -S " + build_dir.string() + " -B " + bin_dir.string();

         return system::execute("cmake -S " + build_dir.string() + " -B " + bin_dir.string()); //cmake_cmd);
      }


      int32_t build() noexcept {
         auto bin_dir = std::filesystem::path(path) / std::filesystem::path("build") / std::filesystem::path("antler-bin");

         std::filesystem::create_directory(bin_dir);
         std::string make_cmd = "cmake --build " + bin_dir.string();

         return system::execute(make_cmd);
      }

      int32_t exec() {
         auto proj = load_project(path);
         if (!proj) {
            return -1;
         }

         if (should_repopulate()) {
            if (!proj->populate()) {
               std::cerr << "Population of the project failed." << std::endl;
               return -1;
            }
            if (auto rv = configure(); rv != 0) {
               std::cerr << "Configuring project build failed!" << std::endl;
            }
         }
         return build();
      }
      
      CLI::App*   subcommand;
      std::string path;
      bool verbose;
   };
} // namespace antler