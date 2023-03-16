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
         subcommand->add_option("-p, path", path, "This is the path to the root of the project.")->default_val(".");
      }

      bool should_repopulate() {
         auto proj = std::filesystem::path(path) / project::project::manifest_name;
         auto build = std::filesystem::path(path) / "build" / project::cmake_lists::filename;

         auto last_manifest_time = std::filesystem::last_write_time(proj);

         if (!std::filesystem::exists(build)) {
            return true;
         }

         auto last_pop_time      = std::filesystem::last_write_time(build);

         return last_pop_time < last_manifest_time;
      }

      int32_t configure() noexcept {
         auto build_dir = std::filesystem::path(path) / "build";
         auto bin_dir = build_dir / "antler-bin";

         std::cout << "CONFDIR " << build_dir << " " << bin_dir << std::endl;

         std::filesystem::create_directory(bin_dir);
         std::string cmake_cmd = "cmake -S " + build_dir.string() + " -B " + bin_dir.string();

         return system::execute("cmake -S " + build_dir.string() + " -B " + bin_dir.string()); //cmake_cmd);
      }


      int32_t build() noexcept {
         auto bin_dir = std::filesystem::path(path) / "build" / "antler-bin";

         std::cout << "BIN " << bin_dir << std::endl;

         std::filesystem::create_directory(bin_dir);
         std::string make_cmd = "cmake --build " + bin_dir.string();

         return system::execute(make_cmd);
      }

      void move_artifacts(const project::project& proj) noexcept {
         namespace sf = std::filesystem;
         auto build_dir = sf::path(path) / "build";
         auto bin_dir = build_dir / "antler-bin";

         for (const auto& [app_nm, app] : proj.apps()) {
            std::string proj_nm = std::string(proj.name());
            auto from_wasm = bin_dir / sf::path(proj.name()) / sf::path(app_nm+"/"+proj_nm+"-"+app_nm+".wasm");
            auto from_abi = bin_dir / sf::path(proj.name()) / sf::path(app_nm+"/"+proj_nm+"-"+app_nm+".abi");

            auto to_wasm = build_dir /  sf::path(app_nm+".wasm");
            auto to_abi = build_dir / sf::path(app_nm+".abi");

            std::filesystem::copy(from_wasm, to_wasm);
            std::filesystem::copy(from_abi, to_abi);

            system::info_log("{0} and {1} have been created.", to_wasm.string(), to_abi.string());
         }
      }

      int32_t exec() {
         try {
            auto proj = load_project(path);

            if (should_repopulate()) {
               project::cmake emitter(proj);
               emitter.emit();

               if (auto rv = configure(); rv != 0) {
                  system::error_log("Configuring project build failed.");
                  return rv;
               }
            }

            if ( auto rv = build(); rv != 0) {
               system::error_log("Building the project failed.");
               return rv;
            }

            move_artifacts(proj);

            return 0;
         } catch (const std::runtime_error& e) {
            system::error_log("Error: {0}", e.what());
            return -1;
         }
      }
      
      CLI::App*   subcommand = nullptr;
      std::string path = "";
   };
} // namespace antler