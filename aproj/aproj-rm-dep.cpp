/// @copyright See `LICENSE` in the root directory of this project.

#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>            // std::sort
#include <CLI11.hpp>

#include <antler/project/project.hpp>

#include <aproj-common.hpp>


template<typename T>
std::string to_string(T list) {
   auto pos = list.begin();
   auto end = list.end();
   std::ostringstream ss;
   ss << "{ ";
   if (pos != end) {
      ss << *pos;
      for (++pos; pos != end; ++pos)
         ss << ", " << *pos;
   }
   ss << " }";
   return ss.str();
}


antler::project::object::list_t populate_update_list(
   const antler::project::object::list_t& src, ///< The list of objects.
   std::string_view dep_name,                  ///< The dependency to remove.
   std::string_view obj_name,                  ///< The name of the object (or empty) to remove from.
   bool app,
   bool lib,
   bool tst) ///< Valid object types to remove from.
   noexcept {

   antler::project::object::list_t rv;
   for (const auto& a : src) {
      switch (a.type()) {
         case antler::project::object::type_t::app:
            if (!app) {
               continue;
            }
            break;
         case antler::project::object::type_t::lib:
            if (!lib) {
               continue;
            }
            break;
         case antler::project::object::type_t::test:
            if (!tst) {
               continue;
            }
            break;
         case antler::project::object::type_t::none:
         case antler::project::object::type_t::any:
            std::cerr << "Unexpected type: " << a.type() << " in object: " << a.name() << '\n';
            continue;
      }

      if ((obj_name.empty() || a.name() == obj_name) && a.dependency_exists(dep_name)) {
         rv.emplace_back(a);
         rv.back().remove_dependency(dep_name);
      }
   }

   return rv;
}


int main(int argc, char** argv) {

   common_init(argc,argv,"Remove a dependency.");

   std::filesystem::path path;
   std::string dep_name;
   std::string obj_name;
   bool rm_all = false;
   bool rm_app = false;
   bool rm_lib = false;
   bool rm_test = false;
   bool interactive=false;


   // Setup CLI.

   CLI::App cli(brief_str,exe_name);

   // Positional arguments:
   cli.add_option("path", path, "This must be the path to `project.yaml` or the path containing it.")->required();
   cli.add_option("dep_name", dep_name, "The name of the dependency to remove.");

   // Options
   cli.add_option("--name", obj_name, "Remove the dep from only this object.");

   // Option flag
   auto all_flag = cli.add_flag("--all", rm_all, "Remove dep_name from all objects (implies --app, --lib, --test). Default option.");
   cli.add_flag("--app", rm_app, "Remove dep_name from application objects.")->excludes(all_flag);
   cli.add_flag("--lib", rm_lib, "Remove dep_name from library objects.")->excludes(all_flag);
   cli.add_flag("--test", rm_test, "Remove dep_name from test objects.")->excludes(all_flag);
   cli.add_flag("--interactive", interactive, "Force interactive mode.");

   // Parse
   CLI11_PARSE(cli,argc,argv);


   // load project or exit.
   auto proj = load_project_or_exit(cli,path);


   // Get all the objects and their names.
   const auto all_objs = proj.all_objects();

   // Find all the object and dependency names.
   std::vector<std::string_view> all_obj_names;
   std::vector<std::string_view> all_dep_names;
   for (const auto& a : all_objs) {
      all_obj_names.emplace_back(a.name());
      for (const auto& b : a.dependencies())
         all_dep_names.emplace_back(b.name());
   }

   // Sort dep name list and make it unique.
   std::sort(all_dep_names.begin(), all_dep_names.end());
   all_dep_names.erase(std::unique(all_dep_names.begin(), all_dep_names.end()), all_dep_names.end());
   // If there aren't any deps, then there's nothing to do...
   if (all_dep_names.empty())
      return cli.exit( CLI::Error("path","project file does not contain any dependencies.") );

   // Sort obj name list and make it unique.
   std::sort(all_obj_names.begin(), all_obj_names.end());
   all_obj_names.erase(std::unique(all_obj_names.begin(), all_obj_names.end()), all_obj_names.end());


   // Set the individual rm flags if all was explicitly set OR implicitly (i.e. no rm flags at all).
   if (rm_all || (!rm_app && !rm_lib && !rm_test)) {
      rm_app = true;
      rm_lib = true;
      rm_test = true;
   }


   antler::project::object::list_t update_list;

   // An existing dep_name indicates a non--interactive mode.
   interactive |= dep_name.empty();
   if (!interactive) {
      // Just create the update list.
      update_list = populate_update_list(all_objs, dep_name, obj_name, rm_app, rm_lib, rm_test);
   }
   else {
      // Get input from the user.
      bool first_time = true;
      antler::project::object::list_t temp0 = populate_update_list(all_objs, dep_name, obj_name, rm_app, rm_lib, rm_test);
      // Loop until the user is happy with the update.
      for (;;) {

         if (temp0.empty()) {
            // This block shows the user the available objects and provides a warning/error if this is the first pass.
            dump_obj_deps(all_objs, rm_app, rm_lib, rm_test);
            if (!first_time)
               std::cerr << "No objects with dependency " << dep_name << "\n\n";
         }
         else {
            // This block provides the current values and queries the user regarding correctness.
            std::cout
               << '\n'
               << "Dependency name: " << dep_name << '\n';
            if (!obj_name.empty())
               std::cout << "Object name:     " << obj_name << '\n';

            std::cout << "Objects that will be updated (" << temp0.size() << "):\n";
            for (auto a : temp0)
               std::cout << "  " << a.name() << " [" << a.type() << "]\n";
            std::cout << '\n';

            if (is_this_correct()) {
               update_list = temp0;
               break;
            }
         }

         first_time = false;

         // dependency name
         {
            auto validator = [&all_dep_names](std::string_view s) -> bool {
               if (validate_name(s) && std::find(all_dep_names.begin(), all_dep_names.end(), s) != all_dep_names.end())
                  return true;
               std::cerr << "Valid dependencies: \n";
               for (auto a : all_dep_names)
                  std::cout << "  " << a << '\n';
               return false;
            };
            get_valid_string("dependency name", dep_name, validator);
         }
         // object name
         get_name("object name", obj_name, true);

         // Update the temporary removal list.
         temp0 = populate_update_list(all_objs, dep_name, obj_name, rm_app, rm_lib, rm_test);
      }
   }

   // Sanity check. This might be better if it returned true in certain circumstances?
   if (update_list.empty())
      return cli.exit( CLI::Error("","no objects were selected for removal.") );

   // Update the objects that need updating.
   for (auto a : update_list)
      proj.upsert(std::move(a));

   // Sync the project to storage.
   if (!proj.sync())
      return cli.exit( CLI::Error("path","failed to write project file.") );
   return 0;
}
