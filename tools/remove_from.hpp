#pragma once

#include <iostream>
#include <string>

#include "CLI11.hpp"

#include <antler/project/project.hpp>

#include "common.hpp"

namespace antler {
   struct remove_from_project {

      template <typename Obj>
      bool remove_obj(antler::project::project& proj) {
         if (obj_name.empty()) {
            system::error_log("Object name is empty.");
            return false;
         }

         if (proj.remove(obj_name)) {
            system::info_log("Object {0} removed from the project.", obj_name);
            return true;
         }
         return false;
      }

      inline bool remove_app(antler::project::project& proj) { return remove_obj<antler::project::app_t>(proj); }
      inline bool remove_lib(antler::project::project& proj) { return remove_obj<antler::project::lib_t>(proj); }
      // TODO reenable remove_test() when test code is completed.
      //inline bool remove_test(antler::project::project& proj) { return remove_obj<antler::project::test_t>(proj); }

      bool remove_dependency_from_all(antler::project::project& proj) {
         const auto& remove_dep = [](auto dep, auto& objs) {
            for (auto& [k, o] : objs) {
               if (o.remove_dependency(dep)) {
                  std::cout << "Removing dependency: " << dep << " from: " << o.name() << std::endl;
               }
            }
         };
         // Get all the objects and their names.
         remove_dep(dep_name, proj.apps());
         remove_dep(dep_name, proj.libs());
         // TODO reenable call to remove_dep() when test code is completed.
         //remove_dep(dep_name, proj.tests());

         return 0;
      }

      template <typename Obj>
      bool remove_dependency(antler::project::project& proj) {
         // Get the object to operate on.
         auto& obj = proj.object<typename Obj::tag_t>(obj_name);

         if (obj.remove_dependency(dep_name)) {
            system::info_log("Dependency {0} removed from object {1}.", dep_name, obj_name);
            return true;
         } else {
            system::error_log("Dependency {0} is not a dependency of object {1}.", dep_name, obj_name);
            return false;
         }
         
         return true;
      }

      remove_from_project(CLI::App& app) {
         path = system::fs::current_path().string();
         subcommand = app.add_subcommand("remove", "Add an app, dependency, library or test to your project.");
         subcommand->add_option("-p, path", path, "This must be the path to the `project.yml` or the path containing it.")->default_val(".");

         app_subcommand = subcommand->add_subcommand("app", "Remove app from the project.");
         app_subcommand->add_option("-n, name", obj_name, "The name of the app to remove.")->required();

         lib_subcommand = subcommand->add_subcommand("lib", "Remove lib from the project.");
         lib_subcommand->add_option("-n, name", obj_name, "The name of the library to add.")->required();

         dep_subcommand = subcommand->add_subcommand("dep", "Remove a dependency from the project.");
         dep_subcommand->add_option("-d, dep", dep_name, "The name of the dependency.")->required();
         dep_subcommand->add_option("-o, obj", obj_name, "The name of the object the dependency is attached to.");

         /* TODO Add back after this release when we have the testing framework finished
         test_subcommand = subcommand->add_subcommand("test", "Remove a test from the project.");
         test_subcommand->add_option("-n, name", dep_name, "The name of the test to remove.")->required();
         */
         
      }

      int32_t exec() {
         auto proj = load_project(path);

         if (*app_subcommand) {
            remove_app(proj);
         } else if (*lib_subcommand) {
            remove_lib(proj);
         } else if (*dep_subcommand) {
            if (obj_name.empty()) {
               remove_dependency_from_all(proj);
            } else {
               if (proj.app_exists(obj_name)) {
                  remove_dependency<antler::project::app_t>(proj);
               } else if (proj.lib_exists(obj_name)) {
                  remove_dependency<antler::project::lib_t>(proj);
               } else {
                  system::error_log("Object {0} does not exist.", obj_name);
               }
            }
         /* TODO Add back after this release when we have the testing framework finished
         } else if (*test_subcommand) {
            remove_test(*proj);
         */
         } else {
            system::error_log("Need to supply either dep/app/lib/test after remove");
            return -1;
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
   };
} // namespace antler
