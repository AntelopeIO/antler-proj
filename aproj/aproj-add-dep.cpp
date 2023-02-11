/// @copyright See `LICENSE` in the root directory of this project.

#include <iostream>
#include <filesystem>
#include <vector>
#include <CLI11.hpp>

#include <antler/project/project.hpp>

#include <aproj-common.hpp>


int main(int argc, char** argv) {

   common_init(argc,argv,"Add a dependency.");

   std::filesystem::path path;
   std::string obj_name = "frog";
   std::string dep_name;
   std::string dep_loc;
   std::string dep_tag;
   std::string dep_rel;
   std::string dep_hash;


   CLI::App cli(brief_str,exe_name);

   // Positional arguments:
   cli.add_option("path", path, "This must be the path to `project.yaml` or the path containing it.")->required();
   cli.add_option("obj_name", obj_name, "The name of the object to receive the dependency `dep_name`.");
   cli.add_option("dep_name", dep_name, "The name of the dependency to add to `obj_name`.");
   cli.add_option("location", dep_loc, "Dependency locationL url, path, or github organization/repo shortcut.");

   // Optional arguments:
   auto tag_opt = cli.add_option("--tag", dep_tag, "The github tag or commit hash; only valid when LOCATION is a github repository.");
   //auto commit_opt = cli.add_option("--commit", dep_commit, "The github tag or commit hash; only valid when LOCATION is a github repository.");
   //auto branch_opt = cli.add_option("--branch", dep_branch, "The github tag or commit hash; only valid when LOCATION is a github repository.");
   cli.add_option("--rel,--release", dep_rel, "A github release version.")->excludes(tag_opt);
   cli.add_option("--hash", dep_hash, "SHA256 hash; only valid when LOCATION gets an archive (i.e. *.tar.gz or similar).");

   // Parse
   CLI11_PARSE(cli,argc,argv);

   // Get the path to the project.
   if (!antler::project::project::update_path(path))
      return cli.exit( CLI::Error("name","path either did not exist or no `project.yaml` file could be found.") );

   // Load the project.
   auto optional_proj = antler::project::project::parse(path);
   if (!optional_proj)
      return cli.exit( CLI::Error("name", "Failed to load project file.") );
   auto proj = optional_proj.value();




   if (argc >= 3) {
      obj_name = argv[2];
      if (!proj.object_exists(obj_name))
         return cli.exit( CLI::Error("name", "OBJ_NAME does not exist in project.") );
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
               return cli.exit( CLI::Error("name", "--tag requires an argument.") );
            ++i;
            dep_tag = next;
         }
         if (temp == "--rel") {
            if (next.empty())
               return cli.exit( CLI::Error("name", "--tag requires an argument.") );
            ++i;
            dep_rel = next;
         }
         if (temp == "--hash") {
            if (next.empty())
               return cli.exit( CLI::Error("name", "--tag requires an argument.") );
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
      return cli.exit( CLI::Error("name", obj_name + " does not exist in project.") );
   auto obj = obj_vec[0];

   // If we are not in interactive mode, test for the pre-existence of the dependency.
   if (!interactive && obj.dependency_exists(dep_name))
      return cli.exit( CLI::Error("name",  dep_name + " already exists for " + obj_name + " in project.") );

   // Validate the location. Redundant for interactive mode, but cheap in human time.
   std::ostringstream ss;
   if (!antler::project::dependency::validate_location(dep_loc, dep_tag, dep_rel, dep_hash, ss))
      return cli.exit( CLI::Error("name", ss.str()) );

   // Create the dependency, store it in the object, and store the object in the roject.
   antler::project::dependency dep;
   dep.set(dep_name, dep_loc, dep_tag, dep_rel, dep_hash);
   obj.upsert_dependency(std::move(dep));
   proj.upsert(std::move(obj));

   // Sync the project to storage.
   if (!proj.sync())
      return cli.exit( CLI::Error("name", "failed to write project file.") );
   return 0;
}
