/// @copyright See `LICENSE` in the root directory of this project.

#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>            // std::sort

#include <antler/project/project.hpp>

#include <aproj-common.hpp>


/// Print usage information to std::cout and return 0 or, optionally - if err is not-empty() - print to std::cerr and return -1.
/// @param err  An error string to print. If empty, print to std::cout and return 0; otherwise std::cerr and return -1.
/// @return 0 if err.empty(); otherwise -1. This value is suitable for using at exit.
int usage(std::string_view err) {

   std::ostream& os = (err.empty() ? std::cout : std::cerr);

   os << exe_name << ": PATH [DEP_NAME] [OPTIONS]\n"
      << "  " << brief_str << '\n'
      << '\n'
      << "Remove dependency from project applications, libraries, and tests.\n"
      << '\n'
      << " PATH is either path to `project.yaml` or the path containing it.\n"
      << " DEP_NAME is the the name of the dependency to remove.\n"
      << '\n'
      << " Options:\n"
      << "  --all            Remove the dep from all objects (implies --app, --lib, test). Default option.\n"
      << "  --app            Remove the dep from application objects.\n"
      << "  --lib            Remove the dep from library objects.\n"
      << "  --test           Remove the dep from test objects.\n"
      << "  --name=OBJ_NAME  Remove the dep from object OBJ_NAME.\n"
      << '\n'
      << " The `project.yaml` object is updated to add a new dependency.\n"
      << '\n'
      << " If DEP_NAME is absent, the user is prompted.\n"
      << '\n';

   if (err.empty())
      return 0;
   os << "Error: " << err << '\n';
   return -1;
}


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

   // Test arg count is valid.
   if (argc < 2)
      return usage("path is required.");

   // Get the path to the project.
   std::filesystem::path path = argv[1];
   if (!antler::project::project::update_path(path))
      return usage("path either did not exist or no `project.yaml` file could be found.");

   // Load the project.
   auto optional_proj = antler::project::project::parse(path);
   if (!optional_proj)
      return usage("Failed to load project file.");
   auto proj = optional_proj.value();

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
      return usage("project file does not contain any dependencies.");


   // Sort obj name list and make it unique.
   std::sort(all_obj_names.begin(), all_obj_names.end());
   all_obj_names.erase(std::unique(all_obj_names.begin(), all_obj_names.end()), all_obj_names.end());


   std::string dep_name;
   bool rm_all = false;
   bool rm_app = false;
   bool rm_lib = false;
   bool rm_test = false;
   std::string obj_name;

   for (int i = 2; i < argc; ++i) {
      std::string_view temp = argv[i];

      if (temp == "--all") {
         rm_all = true;
         if (rm_lib || rm_app || rm_test)
            return usage("--all conflicts with the flags --app, --lib, and --test.");
         continue;
      }

      if (temp == "--app") {
         rm_app = true;
         if (rm_all)
            return usage("--all conflicts with the flags --app, --lib, and --test.");
         continue;
      }

      if (temp == "--lib") {
         rm_lib = true;
         if (rm_all)
            return usage("--all conflicts with the flags --app, --lib, and --test.");
         continue;
      }

      if (temp == "--test") {
         rm_test = true;
         if (rm_all)
            return usage("--all conflicts with the flags --app, --lib, and --test.");
         continue;
      }

      if (temp.starts_with("--name=")) {
         obj_name = temp.substr(7);
         if (obj_name.empty())
            return usage("--name=OBJ_NAME argument requires a value.");
         continue;
      }

      // All the flags have been looked at, all we have left is a single dependency name.
      if (!dep_name.empty())
         return usage("Received multiple dependency names, only one is allowed.");
      dep_name = temp;
   }

   // Set the individual rm flags if all was explicitly set OR implicitly (i.e. no rm flags at all).
   if (rm_all || (!rm_app && !rm_lib && !rm_test)) {
      rm_app = true;
      rm_lib = true;
      rm_test = true;
   }


   antler::project::object::list_t update_list;

   // An existing dep_name indicates a non--interactive mode.
   if (!dep_name.empty()) {
      // So just update the list.
      update_list = populate_update_list(all_objs, dep_name, obj_name, rm_app, rm_lib, rm_test);
   } else {
      // Get input from the user.
      bool first_time = true;
      antler::project::object::list_t temp0 = populate_update_list(all_objs, dep_name, obj_name, rm_app, rm_lib, rm_test);

      for (;;) {

         if (temp0.empty()) {
            dump_obj_deps(all_objs, rm_app, rm_lib, rm_test);
            if (!first_time)
               std::cerr << "No objects with dependency " << dep_name << "\n\n";
         } else {
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
         for (;;) {
            get_name("dependency name", dep_name);
            if (std::find(all_dep_names.begin(), all_dep_names.end(), dep_name) == all_dep_names.end()) {
               std::cerr << "Valid dependencies: \n";
               for (auto a : all_dep_names)
                  std::cout << "  " << a << '\n';
               continue;
            }
            break;
         }

         // object name
         get_name("object name", obj_name, true);

         // Update the temporary removal list.
         temp0 = populate_update_list(all_objs, dep_name, obj_name, rm_app, rm_lib, rm_test);
      }
   }

   // Sanity check. This might be better if it returned true in certain circumstances?
   if (update_list.empty())
      return usage("no objects were selected for removal.");

   // Update the objects that need updating.
   for (auto a : update_list)
      proj.upsert(std::move(a));

   // Sync the project to storage.
   if (!proj.sync())
      return usage("failed to write project file.");
   return 0;
}
