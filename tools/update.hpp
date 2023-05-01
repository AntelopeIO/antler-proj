#pragma once

#include <iostream>
#include <string>

#include "CLI11.hpp"

#include <antler/project/project.hpp>

#include "common.hpp"

namespace antler {
   struct update_project {

      template <typename Obj>
      bool update_obj(antler::project::project& proj, CLI::App& app) {
         if (obj_name.empty()) {
            system::error_log("Object name is empty");
            return false;
         }

         try {
            auto& obj = proj.object<typename Obj::tag_t>(obj_name);
            const auto* lang_opt = app.get_option_no_throw("language");
            const auto* copts_opt = app.get_option_no_throw("compile_options");
            const auto* lopts_opt = app.get_option_no_throw("link_options");


            if (!lang_opt->empty())
               obj.language(lang);

            if (!copts_opt->empty())
               obj.compile_options(copts);
            if (!lopts_opt->empty())
               obj.link_options(lopts);

            system::info_log("Updating object: {0}\n"
                             "language: {1}\n"
                             "compile options: {2}\n"
                             "link options: {3}\n",
                             obj_name,
                             obj.language(),
                             copts,
                             lopts);

            return true;
         } catch(...) {
            system::error_log("Object {0} does not exist in the project", obj_name);
         }
         return false;
      }

      inline bool update_app(antler::project::project& proj) { return update_obj<antler::project::app_t>(proj, *app_subcommand); }
      inline bool update_lib(antler::project::project& proj) { return update_obj<antler::project::lib_t>(proj, *lib_subcommand); }
      //inline bool update_test(antler::project::project& proj) { return update_obj<antler::project::test_t>(proj, *test_subcommand); }

      template <typename Obj>
      bool update_dependency(antler::project::project& proj, const std::string& obj_n) {
         const auto* loc_opt = dep_subcommand->get_option_no_throw("location");
         const auto* tag_opt = dep_subcommand->get_option_no_throw("tag");
         const auto* rel_opt = dep_subcommand->get_option_no_throw("release");
         const auto* dig_opt = dep_subcommand->get_option_no_throw("hash");

         try {
            auto& obj = proj.object<typename Obj::tag_t>(obj_n);
            auto  dep = obj.find_dependency(dep_name);

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
                  system::error_log("Invalid dependency: {0}\n", dep_name);
                  return false;
               }
               system::info_log("Updating dependency: {0} for object: {1}", dep_name, obj.name());
               obj.upsert_dependency(std::move(*dep));
               return true;
            } else {
               system::error_log("Dependency {0} not found in object {1", dep_name, obj_name);
            }
         } catch(...) {
            system::error_log("Object {0} does not exist in the project", obj_name);
         }
         return false;
      }

      void update_dependency_for_all(antler::project::project& proj) {
         const auto& update_dep = [&](auto dep, auto& objs) {
            for (auto& [k, o] : objs) {
               using obj_t = std::decay_t<decltype(o)>;
               if (o.dependency_exists(dep_name)) {
                  update_dependency<obj_t>(proj, o.name());
               }
            }
         };

         // Get all the objects and their names.
         update_dep(dep_name, proj.apps());
         update_dep(dep_name, proj.libs());
         //update_dep(dep_name, proj.tests());
      }

      update_project(CLI::App& app) {
         path = system::fs::current_path().string();
         subcommand = app.add_subcommand("update", "Update an app, dependency, library or test in your project.");

         subcommand->add_option("-p, path", path, "Path containing the project's yaml file.");

         app_subcommand = subcommand->add_subcommand("app", "Update an app in the project.");
         app_subcommand->footer(std::string(R"(Examples:)")
               + "\n\t" + app.get_name() +R"( update app MyApp C++ \\-O2)"
               + "\n\t" + app.get_name() +R"( update ./path-to-project/ app -n MyApp --comp -O2)");
         app_subcommand->add_option("-p", path, "Path containing the project's yaml file.");
         app_subcommand->add_option("-n, name", obj_name, "The name of the app to remove.")->required();
         app_subcommand->add_option("-l, language", lang, "The language of the app.");
         app_subcommand->add_option("--comp, compile_options", copts, "The compile options used to build the app.")
            ->transform(escape_transform);
         app_subcommand->add_option("--link, link_options", lopts, "The link options used to build the app.")
            ->transform(escape_transform);

         lib_subcommand = subcommand->add_subcommand("lib", "Update a lib in the project.");
         lib_subcommand->footer(std::string(R"(Examples:)")
               + "\n\t" + app.get_name() +R"( update lib MyLib C++ \\-O2 "\-s")"
               + "\n\t" + app.get_name() +R"( update lib MyLib --link -s)");
         lib_subcommand->add_option("-p", path, "Path containing the project's yaml file.");
         lib_subcommand->add_option("-n, name", obj_name, "The name of the library to add.")->required();
         lib_subcommand->add_option("-l, language", lang, "The language of the lib.");
         lib_subcommand->add_option("--comp, compile_options", copts, "The compile options used to build the app.")
            ->transform(escape_transform);
         lib_subcommand->add_option("--link, link_options", lopts, "The link options used to build the app.")
            ->transform(escape_transform);

         dep_subcommand = subcommand->add_subcommand("dep", "Update a dependency in the project.");
         dep_subcommand->footer(std::string(R"(Examples:)")
               + "\n\t" + app.get_name() +R"( update dep MyDep -l AntelopeIO/my_dep)");
         dep_subcommand->add_option("-p", path, "Path containing the project's yaml file.");
         dep_subcommand->add_option("-d, dep", dep_name, "The name of the dependency.")->required();
         dep_subcommand->add_option("-o, object", obj_name, "The name of the object the dependency is attached to.");
         dep_subcommand->add_option("-l, location", loc, "The location of the dependency.");
         auto* tag_opt = dep_subcommand->add_option("-t, tag", tag, "The github tag or commit hash; only valid when LOCATION is a github repository.");
         dep_subcommand->add_option("-r, release", release, "A github release version.")->excludes(tag_opt);
         dep_subcommand->add_option("--digest, hash", digest, "SHA256 message digest; only valid when LOCATION gets an archive (i.e. *.tar.gz or similar).");

         /* TODO Add back after this release when we have the testing framework finished
         test_subcommand = subcommand->add_subcommand("test", "Remove a test from the project.");
         test_subcommand->footer(std::string(R"(Examples:)")
               + "\n\t" + app.get_name() +R"( update dep MyDep -l AntelopeIO/my_dep")");
         test_subcommand->add_option("-p", path, "Path containing the project's yaml file.");
         test_subcommand->add_option("-p", path, "Path containing the project's yaml file.");
         test_subcommand->add_option("-n, name", dep_name, "The name of the test to remove.")->required();
         */

      }

      int32_t exec() {
         auto proj = load_project(path);

         if (*app_subcommand) {
            update_app(proj);
         } else if (*lib_subcommand) {
            update_lib(proj);
         } else if (*dep_subcommand) {
            if (!obj_name.empty()) {
               if (proj.app_exists(obj_name))
                  update_dependency<antler::project::app_t>(proj, obj_name);
               else if (proj.lib_exists(obj_name))
                  update_dependency<antler::project::lib_t>(proj, obj_name);
            } else {
               update_dependency_for_all(proj);
            }
         /* TODO Add back after this release when we have the testing framework finished
         } else if (*test_subcommand) {
            update_test(*proj);
         */
         } else {
            system::error_log("Need to supply either a dep/app/lib/test after `update`");
            return -1;
         }

         proj.sync();
         return 0;
      }

      CLI::App*   subcommand = nullptr;
      CLI::App*   app_subcommand = nullptr;
      CLI::App*   dep_subcommand = nullptr;
      CLI::App*   lib_subcommand = nullptr;
      CLI::App*   test_subcommand = nullptr;
      std::string path = "";
      std::string obj_name = "";
      std::string dep_name = "";
      std::string lang = "";
      std::string copts = "";
      std::string lopts = "";
      std::string loc = "";
      std::string tag = "";
      std::string release = "";
      std::string digest = "";
   };
} // namespace antler
