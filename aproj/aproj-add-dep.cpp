/// @copyright See `LICENSE` in the root directory of this project.

#include <iostream>
#include <filesystem>
#include <vector>

#include <antler/project/project.hpp>

#include <aproj-common.hpp>


/// Print usage information to std::cout and return 0 or, optionally - if err is not-empty() - print to std::cerr and return -1.
/// @param err  An error string to print. If empty, print to std::cout and return 0; otherwise std::cerr and return -1.
/// @return 0 if err.empty(); otherwise -1. This value is suitable for using at exit.
int usage(std::string_view err) {

   std::ostream& os = (err.empty() ? std::cout : std::cerr);

   os << exe_name << ": PATH [OBJECT_NAME DEP_NAME [LOCATION [options]]]\n"
      << "  " << brief_str << '\n'
      << '\n'
      << " PATH is either path to `project.yaml` or the path containing it.\n"
      << " OBJ_NAME is the the name of the object to receive DEP_NAME.\n"
      << " DEP_NAME is the the name of this dependency.\n"
      << " LOCATION is either a path or URL for finding this dependency.\n"
      << '\n'
      << " Options:\n"
      << "  --tag     The github tag or commit hash; only valid when LOCATION is a github repository.\n"
      << "  --rel     The github version for LOCATION.\n"
      << "  --hash    SHA256 hash; only valid when LOCATION gets an archive (i.e. *.tar.gz or similar).\n"
      << "  --help    Print this help and exit.\n"
      << '\n'
      << " The `project.yaml` object is updated to add a new dependency.\n"
      << '\n'
      << " If either OBJECT_NAME or DEP_NAME is absent, the user is prompted.\n"
      << '\n';

   if (err.empty())
      return 0;
   os << "Error: " << err << '\n';
   return -1;
}


int main(int argc, char** argv) {

   common_init(argc,argv,"Add a dependency.");

   // Test arg count is valid.
   if (argc < 2)
      return usage("path is required.");
   if (argc > 9)
      return usage("too many options.");

   // Get the path to the project.
   std::filesystem::path path = argv[1];
   if (!antler::project::project::update_path(path))
      return usage("path either did not exist or no `project.yaml` file could be found.");

   // Load the project.
   auto optional_proj = antler::project::project::parse(path);
   if (!optional_proj)
      return usage("Failed to load project file.");
   auto proj = optional_proj.value();


   std::string obj_name;
   std::string dep_name;
   std::string dep_loc;
   std::string dep_tag;
   std::string dep_rel;
   std::string dep_hash;


   if (argc >= 3) {
      obj_name = argv[2];
      if (!proj.object_exists(obj_name))
         return usage("OBJ_NAME does not exist in project.");
   }

   if (argc >= 4)
      dep_name = argv[3];

   if (argc >= 5)
      dep_loc = argv[4];

   if (argc >= 6) {
      for (int i = 5; i < argc; ++i) {
         std::string_view temp = argv[i];
         std::string_view next;
         if (i + 1 < argc)
            next = argv[i + 1];
         if (temp == "--tag") {
            if (next.empty())
               return usage("--tag requires an argument.");
            ++i;
            dep_tag = next;
         }
         if (temp == "--rel") {
            if (next.empty())
               return usage("--tag requires an argument.");
            ++i;
            dep_rel = next;
         }
         if (temp == "--hash") {
            if (next.empty())
               return usage("--tag requires an argument.");
            ++i;
            dep_hash = next;
         }
      }
   }

   // Assuming interactive mode.
   const bool interactive = dep_name.empty();
   if (interactive) {
      for (;;) {
         if (!obj_name.empty() && !dep_name.empty() && antler::project::dependency::validate_location(dep_loc, dep_tag, dep_rel, dep_hash)) {
            // Get the object to operate on.
            auto obj_opt = proj.object(obj_name);

            // If it doesn't exist, none of the existing values can be correct, so alert and jump straigt to queries.
            if (obj_opt.empty())
               std::cerr << obj_name << " does not exist in project.\n";
            else {
               std::cout
                  << '\n'
                  << "Object name (to update): " << obj_name << '\n'
                  << "Dependency name:         " << dep_name << '\n'
                  << "Dependency location:     " << dep_loc << '\n'
                  << "tag/commit hash:         " << dep_tag << '\n'
                  << "release version:         " << dep_rel << '\n'
                  << "SHA256 hash:             " << dep_hash << '\n'
                  << '\n';

               // Get object here and warn user if dep_name already exists.
               auto obj = obj_opt[0];
               if (!dep_name.empty() && obj.dependency_exists(dep_name))
                  std::cerr << dep_name << " already exists for " << obj_name << " in project.\n";

               if (is_this_correct())
                  break;
            }
         }

         // here we want to test that object name exists before we go on.
         std::vector<antler::project::object> obj_vec;
         for (;;) {
            get_name("object (app/lib/test) name", obj_name);
            obj_vec = proj.object(obj_name);
            if (obj_vec.empty()) {
               std::cerr << obj_name << " does not exist in " << proj.name() << '\n';
               continue;
            }
            break;
         }

         // here we want to validate dep name before we go on.
         for (;;) {
            get_name("dependency name", dep_name);
            auto obj = obj_vec[0];
            if (!dep_name.empty() && obj.dependency_exists(dep_name)) {
               std::cerr << dep_name << " already exists for " << obj_name << " in project.\n";
               continue;
            }
            break;
         }

         get_loc("from/location", dep_loc, true);
         get_name("git tag/commit hash", dep_tag, true);
         get_name("git release version", dep_rel, true);
         get_hash("SHA-256 hash", dep_hash, true);
      }
   }

   // Get the object to update.
   auto obj_vec = proj.object(obj_name);
   if (obj_vec.empty())
      RETURN_USAGE(<< obj_name << " does not exist in project.");
   auto obj = obj_vec[0];

   // If we are not in interactive mode, test for the pre-existence of the dependency.
   if (!interactive && obj.dependency_exists(dep_name))
      RETURN_USAGE(<< dep_name << " already exists for " << obj_name << " in project.");

   // Validate the location. Redundant for interactive mode, but cheap in human time.
   std::ostringstream ss;
   if (!antler::project::dependency::validate_location(dep_loc, dep_tag, dep_rel, dep_hash, ss))
      return usage(ss.str());

   // Create the dependency, store it in the object, and store the object in the roject.
   antler::project::dependency dep;
   dep.set(dep_name, dep_loc, dep_tag, dep_rel, dep_hash);
   obj.upsert_dependency(std::move(dep));
   proj.upsert(std::move(obj));

   // Sync the project to storage.
   if (!proj.sync())
      return usage("failed to write project file.");
   return 0;
}
