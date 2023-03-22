#pragma once

#include <iostream>
#include <string>

#include "CLI11.hpp"

#include <antler/project/populator.hpp>

#include "common.hpp"

namespace antler {

   struct build_project {
      inline build_project(CLI::App& app) {
         subcommand = app.add_subcommand("build", "Build a project.");
         subcommand->add_option("-p, path", path, "This is the path to the root of the project.")->default_val(".");
         subcommand->add_option("-j, --jobs", jobs, "This is the path to the root of the project.")->default_val(1);
      }

      bool should_repopulate() {
         auto proj = system::fs::path(path) / project::project::manifest_name;
         auto build = system::fs::path(path) / "build" / project::cmake_lists::filename;

         auto last_manifest_time = system::fs::last_write_time(proj);

         if (!system::fs::exists(build)) {
            return true;
         }

         auto last_pop_time      = system::fs::last_write_time(build);

         return last_pop_time < last_manifest_time;
      }

      int32_t configure() noexcept {
         auto build_dir = system::fs::path(path) / "build";
         auto bin_dir = build_dir / "antler-bin";
         system::info_log("Configuring project...");

         system::fs::create_directory(bin_dir);

         return system::execute("cmake", {"-S", build_dir.string(), "-B", bin_dir.string()});
      }


      int32_t build() noexcept {
         auto bin_dir = system::fs::path(path) / "build" / "antler-bin";

         system::fs::create_directory(bin_dir);
         system::info_log("Building project...");

         return system::execute("cmake", {"--build", bin_dir.string(), "-j", std::to_string(jobs)});
      }

      void move_artifacts(const project::project& proj) noexcept {
         namespace sf = system::fs;
         auto build_dir = sf::path(path) / "build";
         auto bin_dir = build_dir / "antler-bin";

         for (const auto& [app_nm, app] : proj.apps()) {
            std::string proj_nm = std::string(proj.name());
            auto from_wasm = bin_dir / sf::path(proj.name()) / sf::path(app_nm+"/"+proj_nm+"-"+app_nm+".wasm");
            auto from_abi = bin_dir / sf::path(proj.name()) / sf::path(app_nm+"/"+proj_nm+"-"+app_nm+".abi");

            auto to_wasm = build_dir /  sf::path(app_nm+".wasm");
            auto to_abi = build_dir / sf::path(app_nm+".abi");
            
            sf::remove(to_wasm);
            sf::remove(to_abi);

            sf::copy(from_wasm, to_wasm);
            sf::copy(from_abi, to_abi);

            system::info_log("{0} and {1} have been created.", to_wasm.string(), to_abi.string());
         }
      }

      int32_t exec() {
         auto proj = load_project(path);

         if (should_repopulate()) {
            ANTLER_CHECK(project::populators::get(proj).populate(jobs), "failed to populate dependencies");

            if (auto rv = configure(); rv != 0) {
               system::error_log("Configuring project build failed.");
               return rv;
            }
         }

         if (auto rv = build(); rv != 0) {
            system::error_log("Building the project failed.");
            return rv;
         }

         move_artifacts(proj);

         return 0;
      }
      
      CLI::App*   subcommand = nullptr;
      std::string path = "";
      uint32_t    jobs = 1;
   };
} // namespace antler