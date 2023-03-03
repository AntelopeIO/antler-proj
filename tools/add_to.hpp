#pragma once

#include <iostream>
#include <string>

#include <CLI11.hpp>

#include <antler/project/project.hpp>

#include "common.hpp"

namespace antler {
   struct add_to_project {

      template <antler::project::object::type_t Ty>
      bool add_obj(antler::project::project& proj) {
         if (!obj_name.empty()) {
            std::cout
               << '\n'
               << "name: " << obj_name << '\n'
               << "language: " << lang << '\n'
               << "options:  " << options << '\n'
               << std::endl;

            if (proj.object_exists(obj_name, Ty)) {
               std::cerr << "Application " << obj_name << " already exists in project. Can't add.\n\n";
               return false;
            }
         }

         auto obj = antler::project::object(Ty, obj_name, lang, options);
         proj.upsert_app(std::move(obj));
         return true;
      }

      inline bool add_app(antler::project::project& proj) { return add_obj<antler::project::object::type_t::app>(proj); }
      inline bool add_lib(antler::project::project& proj) { return add_obj<antler::project::object::type_t::lib>(proj); }

      bool add_test(antler::project::project& proj) {
         if (!obj_name.empty()) {

            // Print values.
            std::cout
               << '\n'
               << "test name: " << obj_name << '\n'
               << "command:   " << cmd << '\n'
               << '\n';

            // Check to see if name is a TEST duplicate.
            if (proj.object_exists(obj_name, antler::project::object::type_t::test)) {
               // Enform user of the duplicate.
               std::cerr << "Test " << obj_name << " already exists in project. Can't add.\n\n";
            } else {
               // Check to see if name is otherwise a duplicate, warn if so.
               if (proj.object_exists(obj_name))
                  std::cerr << "WARNING: " << obj_name << " already exists in project as app and/or lib.\n\n";
            }
            return true;
         }
         return false;
      }

      bool add_dependency(antler::project::project& proj) {
         if (!obj_name.empty() && !dep_name.empty() && antler::project::dependency::validate_location(location, tag, release, hash)) {
            // Get the object to operate on.
            try {
               auto& obj = proj.object(obj_name);

               // We have values, so query the user if they want to apply.
               std::cout
                  << '\n'
                  << "Object name (to update): " << obj_name << '\n'
                  << "Dependency name:         " << dep_name << '\n'
                  << "Dependency location:     " << location << '\n'
                  << "tag/commit hash:         " << tag << '\n'
                  << "release version:         " << release << '\n'
                  << "SHA256 hash:             " << hash << '\n'
                  << std::endl;

               // Get object here and warn user if dep_name already exists.
               if (!dep_name.empty() && obj.dependency_exists(dep_name)) {
                  std::cerr << dep_name << " already exists for " << obj_name << " in project.\n";
                  return false;
               }

               antler::project::dependency dep;
               dep.set(dep_name, location, tag, release, hash);
               obj.upsert_dependency(std::move(dep));
               //proj.upsert(std::move(obj_vec[0]));

            } catch(...) {
               std::cerr << "Object: " << obj_name << " does not exist." << std::endl;
            }
         }

         return true;
      }

      add_to_project(CLI::App& app) {
         path = std::filesystem::current_path().string();
         subcommand = app.add_subcommand("add", "Add an app, dependency, library or test to your project.");
         subcommand->add_option("-p, path", path, "This is the root path to create the project in.");

         app_subcommand = subcommand->add_subcommand("app", "Add a new app to your project.");
         app_subcommand->add_option("-n, name", obj_name, "The name of the app to add.")->required();
         app_subcommand->add_option("-l, lang", lang, "Language this app will use.")->required();
         app_subcommand->add_option("-o, opts", options, "Options for the compiler for this app.");

         lib_subcommand = subcommand->add_subcommand("lib", "Add a new library to your project.");
         lib_subcommand->add_option("-n, name", obj_name, "The name of the library to add.")->required();
         lib_subcommand->add_option("-l, lang", lang, "Language this library will use.")->required();
         lib_subcommand->add_option("-o, opts", options, "Options for the compiler for this library.");

         dep_subcommand = subcommand->add_subcommand("dep", "Add a new dependency to the project.");
         dep_subcommand->add_option("-o, obj", obj_name, "The name of the object to attach dependency to.")->required();
         dep_subcommand->add_option("-d, dep", dep_name, "The name of the dependency.")->required();
         dep_subcommand->add_option("-l, loc", location, "Location of the dependency.")->required();
         dep_subcommand->add_option("-t, tag", tag, "Tag associated with the dependency.");
         dep_subcommand->add_option("-r, release", release, "Release version of the depedency.");
         dep_subcommand->add_option("--digest, hash", hash, "Hash of the dependency.");
         
         /* TODO Add back after this release when we have the testing framework finished
         test_subcommand = subcommand->add_subcommand("test", "Add a new test to the project.");
         test_subcommand->add_option("-n, name", obj_name, "The name of the test to add.")->required();
         test_subcommand->add_option("-c, command", cmd, "The test command to execute.");
         */
      }

      int32_t exec() {
         try {
            auto proj = load_project(path);

            if (!proj) {
               return -1;
            }

            if (*app_subcommand) {
               add_app(*proj);
            } else if (*lib_subcommand) {
               add_lib(*proj);
            } else if (*dep_subcommand) {
               add_dependency(*proj);
            /* TODO Add back after this release when we have the testing framework finished
            } else if (*test_subcommand) {
               add_test(*proj);
            */
            } else {
               std::cerr << "Need to supply either dep/app/lib/test after `add`" << std::endl;
               return -1;
            }

            proj->sync();
         } catch (...) {
            std::cerr << "Path <" << path << "> does not exist" << std::endl;
         }
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
      std::string options;
      std::string cmd;
   };
} // namespace antler