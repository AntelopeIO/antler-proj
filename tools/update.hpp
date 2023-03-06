#pragma once

#include <iostream>
#include <string>

#include "CLI11.hpp"

#include <antler/project/project.hpp>

#include "common.hpp"

namespace antler {
   struct update_project {

      template <antler::project::object::type_t Ty>
      bool update_obj(antler::project::project& proj, CLI::App& app) {
         if (obj_name.empty()) {
            std::cerr << "Name is empty" << std::endl;
            return false;
         }

         try {
            auto& obj = proj.object(obj_name);
            const auto* lang_opt = app.get_option_no_throw("language");
            const auto* copts_opt = app.get_option_no_throw("compile_options");
            const auto* lopts_opt = app.get_option_no_throw("link_options");


            if (!lang_opt->empty())
               obj.language(lang);

            if (!copts_opt->empty())
               obj.compile_options(copts);
            if (!lopts_opt->empty())
               obj.link_options(lopts);

            std::cout << "Updating object: " << obj_name << "\n"
                      << "language: " << obj.language() << "\n"
                      << "compile options: " << copts << "\n"
                      << "link options: " << lopts << std::endl;

            return true;
         } catch(...) {
            std::cerr << "Object: " << obj_name << " does not exist." << std::endl;
         }
         return false;
      }

      inline bool update_app(antler::project::project& proj) { return update_obj<antler::project::object::type_t::app>(proj, *app_subcommand); }
      inline bool update_lib(antler::project::project& proj) { return update_obj<antler::project::object::type_t::lib>(proj, *lib_subcommand); }
      inline bool update_test(antler::project::project& proj) { return update_obj<antler::project::object::type_t::test>(proj, *test_subcommand); }

      bool update_dependency(antler::project::project& proj, std::string_view obj_n) {
         const auto* loc_opt = dep_subcommand->get_option_no_throw("location");
         const auto* tag_opt = dep_subcommand->get_option_no_throw("tag");
         const auto* rel_opt = dep_subcommand->get_option_no_throw("release");
         const auto* dig_opt = dep_subcommand->get_option_no_throw("hash");

         try {
            auto& obj = proj.object(obj_n);
            auto  dep = obj.dependency(dep_name);

            if (!loc_opt->empty())
               dep->location(loc);
            if (!tag_opt->empty())
               dep->tag(tag);
            if (!rel_opt->empty())
               dep->release(release);
            if (!dig_opt->empty())
               dep->hash(digest);

            if (dep) {
               if (!proj.validate_dependency(*dep)) {
                  std::cerr << "Dependency: " << dep_name << " is invalid." << std::endl;
                  return false;
               }
               std::cout << "Updating dependency: " << dep_name << " for object: " << obj.name() << std::endl;
               obj.upsert_dependency(std::move(*dep));
               return true;
            } else {
               std::cerr << "Dependency: " << dep_name << " not found in: " << obj_name << std::endl;
            }
         } catch(...) {
            std::cerr << "Object: " << obj_name << " not found in project." << std::endl;
         }
         return false;
      }

      bool update_dependency(antler::project::project& proj) {
         const auto* obj_opt = dep_subcommand->get_option_no_throw("object");
         if (!obj_opt->empty()) {
            return update_dependency(proj, obj_name);
         } else {
            const auto& update_dep = [&](auto dep, auto& objs) {
               for (auto& o : objs) {
                  if (o.dependency_exists(dep_name)) {
                     update_dependency(proj, o.name());
                  }
               }
            };

            // Get all the objects and their names.
            update_dep(dep_name, proj.apps());
            update_dep(dep_name, proj.libs());
            update_dep(dep_name, proj.tests());
         }
         return true;
      }

      update_project(CLI::App& app) {
         path = std::filesystem::current_path().string();
         subcommand = app.add_subcommand("update", "Update an app, dependency, library or test to your project.");
         subcommand->add_option("-p, path", path, "This must be the path to the `project.yml` or the path containing it.");

         app_subcommand = subcommand->add_subcommand("app", "Remove app from the project.");
         app_subcommand->add_option("-n, name", obj_name, "The name of the app to remove.")->required();
         app_subcommand->add_option("-l, language", lang, "The language of the app.");
         app_subcommand->add_option("--comp, compile_options", copts, "The compile options used to build the app.");
         app_subcommand->add_option("--link, link_options", lopts, "The link options used to build the app.");

         lib_subcommand = subcommand->add_subcommand("lib", "Remove lib from the project.");
         lib_subcommand->add_option("-n, name", obj_name, "The name of the library to add.")->required();
         lib_subcommand->add_option("-l, language", lang, "The language of the lib.");
         lib_subcommand->add_option("--comp, compile_options", copts, "The compile options used to build the app.");
         lib_subcommand->add_option("--link, link_options", lopts, "The link options used to build the app.");

         dep_subcommand = subcommand->add_subcommand("dep", "Remove a dependency from the project.");
         dep_subcommand->add_option("-d, dep", dep_name, "The name of the dependency.")->required();
         dep_subcommand->add_option("-o, object", obj_name, "The name of the object the dependency is attached to.");
         dep_subcommand->add_option("-l, location", loc, "The location of the dependency.");
         auto* tag_opt = dep_subcommand->add_option("-t, tag", tag, "The github tag or commit hash; only valid when LOCATION is a github repository.");
         dep_subcommand->add_option("-r, release", release, "A github release version.")->excludes(tag_opt);
         dep_subcommand->add_option("--digest, hash", digest, "SHA256 message digest; only valid when LOCATION gets an archive (i.e. *.tar.gz or similar).");

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
               update_app(*proj);
            } else if (*lib_subcommand) {
               update_lib(*proj);
            } else if (*dep_subcommand) {
               update_dependency(*proj);
            /* TODO Add back after this release when we have the testing framework finished
            } else if (*test_subcommand) {
               update_test(*proj);
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
      std::string lang;
      std::string copts;
      std::string lopts;
      std::string loc;
      std::string tag;
      std::string release;
      std::string digest;
   };
} // namespace antler