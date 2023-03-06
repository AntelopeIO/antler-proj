#pragma once

#include <iostream>
#include <string>

#include "CLI11.hpp"

#include <antler/project/project.hpp>

#include "common.hpp"

namespace antler {
   struct remove_from_project {

      template <antler::project::object::type_t Ty>
      bool remove_obj(antler::project::project& proj) {
         if (obj_name.empty()) {
            std::cerr << "Name is empty" << std::endl;
            return false;
         }

         if (proj.remove(obj_name, Ty)) {
            std::cout << "Removing object: " << obj_name << " from the project." << std::endl;
            return true;
         }
         return false;
      }

      inline bool remove_app(antler::project::project& proj) { return remove_obj<antler::project::object::type_t::app>(proj); }
      inline bool remove_lib(antler::project::project& proj) { return remove_obj<antler::project::object::type_t::lib>(proj); }
      inline bool remove_test(antler::project::project& proj) { return remove_obj<antler::project::object::type_t::test>(proj); }

      bool remove_dependency(antler::project::project& proj) {
         if (!dep_name.empty()) {
            if (!obj_name.empty()) {
               // Get the object to operate on.
               try {
                  auto& obj = proj.object(obj_name);

                  if (obj.remove_dependency(dep_name)) {
                     std::cout << "Removing dependency: " << dep_name << " from: " << obj_name << std::endl;
                     return true;
                  } else {
                     std::cerr << "Dependency: " << dep_name << " is not a dependency of: " << obj_name << std::endl;
                     return false;
                  }
               } catch(...) {
                  std::cerr << "Object: " << obj_name << " does not exist." << std::endl;
               }
            } else {
               const auto& remove_dep = [](auto dep, auto& objs) {
                  for (auto& o : objs) {
                     if (o.remove_dependency(dep)) {
                        std::cout << "Removing dependency: " << dep << " from: " << o.name() << std::endl;
                     }
                  }
               };
               // Get all the objects and their names.
               remove_dep(dep_name, proj.apps());
               remove_dep(dep_name, proj.libs());
               remove_dep(dep_name, proj.tests());
            }
         }

         return true;
      }

      remove_from_project(CLI::App& app) {
         path = std::filesystem::current_path().string();
         subcommand = app.add_subcommand("remove", "Add an app, dependency, library or test to your project.");
         subcommand->add_option("-p, path", path, "This must be the path to the `project.yml` or the path containing it.")->required();

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
         try {
            auto proj = load_project(path);

            if (!proj) {
               return -1;
            }

            if (*app_subcommand) {
               remove_app(*proj);
            } else if (*lib_subcommand) {
               remove_lib(*proj);
            } else if (*dep_subcommand) {
               remove_dependency(*proj);
            /* TODO Add back after this release when we have the testing framework finished
            } else if (*test_subcommand) {
               remove_test(*proj);
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
   };
} // namespace antler