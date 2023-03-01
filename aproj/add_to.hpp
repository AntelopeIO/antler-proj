#pragma once

#include <iostream>
#include <string>

#include <CLI11.hpp>

#include <antler/project/project.hpp>

enum class add_to_ty {
   app,
   dep,
   lib,
   test
};

struct add_to_project {
   bool add_dependency(antler::project::project& proj) {
      if (!obj_name.empty() && !dep_name.empty() && antler::project::dependency::validate_location(location, tag, release, hash)) {
         // Get the object to operate on.
         auto obj_vec = proj.object(obj_name);

         // If it doesn't exist, none of the existing values can be correct, so alert and jump straigt to individual queries.
         if (obj_vec.empty()) {
            std::cerr << obj_name << " does not exist in project.\n";
            return false;
         }
         else {
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
            auto obj = obj_vec[0];
            if (!dep_name.empty() && obj.dependency_exists(dep_name)) {
               std::cerr << dep_name << " already exists for " << obj_name << " in project.\n";
               return false;
            }
         }

         antler::project::dependency dep;
         dep.set(dep_name, location, tag, release, hash);
         obj_vec[0].upsert_dependency(std::move(dep));
         proj.upsert(std::move(obj_vec[0]));
      }

      proj.sync();
      return true;
   }

   inline add_to_project(CLI::App& app) {
      path = std::filesystem::current_path().string();
      subcommand = app.add_subcommand("add", "Add an app, dependency, library or test to your project.");
      subcommand->add_option("-p, --path", path, "This is the root path to create the project in.");
      
      dep_subcommand = subcommand->add_subcommand("dep", "Add a new dependency to the project.");
      dep_subcommand->add_option("-o, obj", obj_name, "The name of the object to attach dependency to.")->required();
      dep_subcommand->add_option("-d, dep", dep_name, "The name of the dependency.")->required();
      dep_subcommand->add_option("-l, loc", location, "Location of the dependency.")->required();
   }

   int32_t exec() {
      try {
         auto p = std::filesystem::canonical(std::filesystem::path(path));
         if (!antler::project::project::update_path(p)) {
            std::cerr << "path either did not exist or no `project.yaml` file could be found." << std::endl;
            return -1;
         }
         std::optional<antler::project::project> proj = antler::project::project::parse(p);

         if (!proj) {
            return -1;
         }

         if (*dep_subcommand) {
            add_dependency(*proj);
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
   add_to_ty   ty;
   std::string path;
   std::string obj_name;
   std::string dep_name;
   std::string location;
   std::string tag;
   std::string release;
   std::string hash;
};