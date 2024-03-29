#pragma once

#include <iostream>
#include <string>
#include <cstdlib>

#include "CLI11.hpp"

#include <antler/project/populator.hpp>

#include "common.hpp"

namespace antler {

struct build_project {
   inline build_project(CLI::App& app) {
      subcommand = app.add_subcommand("build", "Build a project.");
      subcommand->footer(std::string(R"(Examples:)") + "\n\t" + app.get_name() + R"( build -j3)");
      subcommand->add_option("-p, path", path, "This is the path to the root of the project.");
      subcommand->add_option("-j, --jobs", jobs, "The number of jobs to use with cmake build tool.");
      subcommand->add_flag("-c, --clean", clean, "This will force a clean build.")->default_val(false);
   }

   int32_t configure() noexcept {
      auto build_dir = system::fs::path(path) / "build";
      auto bin_dir   = build_dir / "antler-bin";
      system::info_log("Configuring project...");

      system::fs::create_directory(bin_dir);

      if (cmake_is_old) {
         std::stringstream ss;
         ss << "cd " << bin_dir.string() << " ; cmake " << build_dir.string() << " ; cmake --build .";
         return system::execute(ss.str(), {});
      }
      return system::execute("cmake", {"-S", build_dir.string(), "-B", bin_dir.string()});
   }


   int32_t build() noexcept {
      auto bin_dir = system::fs::path(path) / "build" / "antler-bin";

      system::fs::create_directory(bin_dir);
      system::info_log("Building project...");

      CLI::results_t args = {"--build", bin_dir.string()};
      if (jobs != std::numeric_limits<uint32_t>::max()) {
         if (cmake_is_old)
            system::warn_log("CMake does not support jobs flag for building.");
         else {
            args.push_back("-j");
            if (jobs)
               args.push_back(std::to_string(jobs));
         }
      }

      return system::execute("cmake", std::move(args));
   }

   void move_artifacts(const project::project& proj) noexcept {
      namespace sf   = system::fs;
      auto build_dir = sf::path(path) / "build";
      auto bin_dir   = build_dir / "antler-bin";

      for (const auto& [app_nm, app] : proj.apps()) {
         std::string proj_nm   = std::string(proj.name());
         auto        from_wasm = bin_dir / sf::path(proj.name()) / sf::path(app_nm + "/" + app_nm + ".wasm");
         auto        from_abi  = bin_dir / sf::path(proj.name()) / sf::path(app_nm + "/" + app_nm + ".abi");

         auto to_wasm = build_dir / sf::path(app_nm + ".wasm");
         auto to_abi  = build_dir / sf::path(app_nm + ".abi");

         sf::remove(to_wasm);
         sf::remove(to_abi);

         sf::copy(from_wasm, to_wasm);
         sf::copy(from_abi, to_abi);

         system::info_log("{0} and {1} have been created.", to_wasm.string(), to_abi.string());
      }
   }

   int32_t exec() {
      // 'cmake --find-package -DNAME=cdt -DCOMPILER_ID=GNU -DLANGUAGE=C -DMODE=EXIST' can be used to decide if cdt and related
      //  *.cmake files are installed, but 'cmake --find-package' will be deprecated, so use 'cdt-cpp --version' instead.
      if (std::system("cdt-cpp --version > /dev/null 2>&1") != 0 ){
          system::error_log("cannot build the project, because CDT is not installed properly in your system!\n"
                           "See https://github.com/AntelopeIO/cdt for the details of installing CDT");
          return -1;
      }
      else {
         system::info_log("Found the CDT compiler tool cdt-cpp");
      }

      cmake_is_old = system::get_cmake_ver() < std::make_tuple(3, 13, 0);

      auto proj = load_project(path);

      system::debug_log("Project loaded at {0}", proj.path().string());

      // reset the path to the root of the project
      path = proj.path().string();

      bool repopulated = false;
      if (should_repopulate(proj)) {
         repopulated = true;
         ANTLER_CHECK(project::populators::get(proj).populate(), "failed to populate dependencies");
      }

      if (clean && !repopulated) {
         system::fs::remove_all(system::fs::path(path) / "build/antler-bin");
      }

      if (auto rv = configure(); rv != 0) {
         system::error_log("Configuring project build failed.");
         return rv;
      }


      if (auto rv = build(); rv != 0) {
         system::error_log("Building the project failed.");
         return rv;
      }

      move_artifacts(proj);

      return 0;
   }

   CLI::App*   subcommand = nullptr;
   std::string path;
   uint32_t    jobs         = std::numeric_limits<uint32_t>::max();
   bool        clean        = false;
   bool        cmake_is_old = false;
};
} // namespace antler
