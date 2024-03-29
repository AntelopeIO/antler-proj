#pragma once

#include <iostream>
#include <string>

#include "CLI11.hpp"

#include <antler/project/project.hpp>
#include <antler/project/source.hpp>

#include "common.hpp"

namespace antler {
struct add_to_project {

   template <typename Obj>
   bool add_obj(antler::project::project& proj) {
      if (proj.exists(obj_name)) {
         system::error_log("Object {0} already exists in project.", obj_name);
         return false;
      }

      if (!Obj::is_valid_name(obj_name)) {
         system::error_log("Object name: {0} is not a valid name.", obj_name);
         system::info_log("Valid names are of the form [a-zA-Z][_a-zA-Z0-9]+");
         return false;
      }

      // TODO for the next release we will remove the C++ restrictions
      const auto& is_valid_cpp_lang = [](auto l) {
         return l == "cpp" ||
                l == "CPP" ||
                l == "c++" ||
                l == "C++" ||
                l == "cxx" ||
                l == "CXX" ||
                l == "Cxx";
      };

      const auto& is_valid_c_lang = [](auto l) {
         return l == "C" ||
                l == "c";
      };

      // we need to produce stub for the app
      if (std::is_same_v<antler::project::app_t, Obj>) {
         if (!is_valid_cpp_lang(lang)) {
            system::error_log("Sorry, as of this version only C++ is available. Given : {0}", lang);
            system::info_log("This restriction will be removed in future releases.");
            return false;
         }
         lang = "CXX";
      } else {
         if (is_valid_cpp_lang(lang))
            lang = "CXX";
         else if (is_valid_c_lang(lang))
            lang = "C";
         else {
            system::error_log("Sorry, as of this version only C or C++ is available. Given : {0}", lang);
            system::info_log("This restriction will be removed in future releases.");
            return false;
         }
      }

      auto obj  = Obj(obj_name, lang, copts, lopts);
      auto path = proj.path();
      system::fs::create_directory(path / project::detail::dir<Obj>() / obj_name);
      system::fs::create_directory(path / system::fs::path("include") / obj_name);
      project::source<Obj>::create_source_file(path, obj);
      project::source<Obj>::create_specification_file(path, obj);

      proj.upsert(std::move(obj));
      system::info_log("\nname: {0}\n"
                       "language: {1}\n"
                       "compile options: {2}\n"
                       "link options: {3}",
                       obj_name,
                       lang,
                       copts,
                       lopts);
      return true;
   }

   inline bool add_app(antler::project::project& proj) { return add_obj<antler::project::app_t>(proj); }
   inline bool add_lib(antler::project::project& proj) { return add_obj<antler::project::lib_t>(proj); }

   bool add_test(antler::project::project& proj) {
      // Check to see if name is otherwise a duplicate, warn if so.
      if (proj.exists(obj_name))
         std::cerr << "WARNING: " << obj_name << " already exists in project as app and/or lib.\n\n";

      std::cout
         << '\n'
         << "test name: " << obj_name << '\n'
         << "command:   " << cmd << '\n'
         << '\n';
      return true;
   }

   template <typename Obj>
   bool add_dependency(antler::project::project& proj) {
      // Get the object to operate on.
      try {
         auto& obj = proj.object<typename Obj::tag_t>(obj_name);

         // Get object here and warn user if dep_name already exists.
         if (!dep_name.empty() && obj.dependency_exists(dep_name)) {
            system::error_log("{0} already exists for in project.", dep_name, obj_name);
            return false;
         }

         antler::project::dependency dep(location, dep_name, tag, release, hash);
         dep_name = dep.name();
         if (!Obj::is_valid_name(dep_name)) {
            system::error_log("Dependency name: {0} is not a valid name.", obj_name);
            system::info_log("Valid names are of the form [a-zA-Z][_a-zA-Z0-9]+");
            return false;
         }
         if (!proj.validate_dependency(dep)) {
            system::error_log("Dependency: {0} is invalid.", dep_name);
            return false;
         }

         obj.upsert_dependency(std::move(dep));

         // We have values, so query the user if they want to apply.
         system::info_log("Adding dependency:\nObject name (to update): {0}\n"
                          "Dependency name: {1}\n"
                          "Dependency location: {2}\n"
                          "tag/commit hash: {3}\n"
                          "release version: {4}\n"
                          "SHA256 hash: {5}",
                          obj_name,
                          dep_name,
                          location,
                          tag,
                          release,
                          hash);

         return true;
      } catch (...) {
         system::error_log("Object {0} does not exist in project.", obj_name);
         return false;
      }
   }

   add_to_project(CLI::App& app) {
      subcommand = app.add_subcommand("add", "Add an app, dependency, library or test to your project.");
      subcommand->add_option("-p,--path", path, "Path containing the project's yaml file.")->default_val(".");

      app_subcommand = subcommand->add_subcommand("app", "Add a new app to your project.");
      app_subcommand->footer(std::string(R"(Examples:)") + "\n\t" + app.get_name() + R"( add app -n MyApp -l C++ "\-O2 -WError" \\-s)" + "\n\t" + app.get_name() + R"( add --path ./path-to-project app -n MyApp -l C++ --comp -O2)");

      app_subcommand->add_option("-n, --name", obj_name, "The name of the app to add.")->required();
      app_subcommand->add_option("-l, --lang", lang, "Language this app will use.")->required();
      app_subcommand->add_option("--comp", copts, "Options for the compiler for this app.")
         ->transform(escape_transform);
      app_subcommand->add_option("--link", lopts, "Options for the linker for this app.")
         ->transform(escape_transform);

      lib_subcommand = subcommand->add_subcommand("lib", "Add a new library to your project.");
      lib_subcommand->footer(std::string(R"(Examples:)") + "\n\t" + app.get_name() + R"( add lib -n MyLib -l C++ --comp -O2 --link -s)");
      lib_subcommand->add_option("-n, --name", obj_name, "The name of the library to add.")->required();
      lib_subcommand->add_option("-l, --lang", lang, "Language this library will use.")->required();
      lib_subcommand->add_option("--comp", copts, "Options for the compiler for this app.")
         ->transform(escape_transform);
      lib_subcommand->add_option("--link", lopts, "Options for the linker for this app.")
         ->transform(escape_transform);

      dep_subcommand = subcommand->add_subcommand("dep", "Add a new dependency to the project.");
      dep_subcommand->footer(std::string(R"(Examples:)") + "\n\t" + app.get_name() + R"( add dep -u https://github.com/org/repo)" + "\n\t" + app.get_name() + R"( add dep --to MyApp --name MyDep -url https://github.com/org/repo)");
      dep_subcommand->add_option("--to", obj_name, "The name of the object to attach dependency to. Default is current directory.");
      dep_subcommand->add_option("-n, --name", dep_name, "The name of the dependency. Default is repo from url.");
      dep_subcommand->add_option("-u, --url", location, "Location of the dependency.");
      dep_subcommand->add_option("-t, --tag", tag, "Tag associated with the dependency.");
      dep_subcommand->add_option("-r, --release_ver", release, "Release version of the depedency.");
      dep_subcommand->add_option("--digest", hash, "Hash of the dependency.");

      /* TODO Add back after this release when we have the testing framework finished
      test_subcommand = subcommand->add_subcommand("test", "Add a new test to the project.");
      test_subcommand->add_option("-p, --path", path, "Path containing the project's yaml file.")->default_val(".");
      test_subcommand->add_option("-n, --name", obj_name, "The name of the test to add.")->required();
      test_subcommand->add_option("-c, --cmd", cmd, "The test command to execute.");
      */
   }

   int32_t exec() {
      auto proj = load_project(path);

      if (*app_subcommand) {
         ANTLER_CHECK(add_app(proj), "failed to add app");
      } else if (*lib_subcommand) {
         ANTLER_CHECK(add_lib(proj), "failed to add lib");
      } else if (*dep_subcommand) {
         if (obj_name.empty()) {
            obj_name = std::filesystem::current_path().filename().c_str();
            system::debug_log("object path is empty, using {0}", obj_name);
         }
         if (proj.app_exists(obj_name)) {
            ANTLER_CHECK(add_dependency<antler::project::app_t>(proj), "failed to add dependency");
         } else if (proj.lib_exists(obj_name)) {
            ANTLER_CHECK(add_dependency<antler::project::lib_t>(proj), "failed to add dependency");
         } else {
            system::error_log("Object {0} does not exist in project.", obj_name);
         }
         /* TODO Add back after this release when we have the testing framework finished
         } else if (*test_subcommand) {
            add_test(*proj);
         */
      } else {
         system::error_log("Need to supply either dep/app/lib/test after `add`");
         return EX_USAGE;
      }

      proj.sync();
      return 0;
   }

   CLI::App*   subcommand;
   CLI::App*   app_subcommand;
   CLI::App*   dep_subcommand;
   CLI::App*   lib_subcommand;
   CLI::App*   test_subcommand;
   std::string path;
   std::string obj_name;
   std::string dep_name;
   std::string location;
   std::string tag;
   std::string release;
   std::string hash;
   std::string lang;
   std::string copts;
   std::string lopts;
   std::string cmd;
};
} // namespace antler
