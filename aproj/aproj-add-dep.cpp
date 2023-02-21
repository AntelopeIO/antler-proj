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
   std::string obj_name;
   std::string dep_name;
   std::string dep_loc;
   std::string dep_tag;
   std::string dep_rel;
   std::string dep_hash;
   bool interactive=false;


   // CLI setup.

   CLI::App cli(brief_str,exe_name);

   // Positional arguments:
   cli.add_option("path", path, "This must be the path to `project.yaml` or the path containing it.")->required();
   cli.add_option("obj_name", obj_name, "The name of the object to receive the dependency `dep_name`.");
   cli.add_option("dep_name", dep_name, "The name of the dependency to add to `obj_name`.");
   cli.add_option("location", dep_loc, "Dependency location url, path, or github organization/repo shortcut.");

   // Optional arguments:
   auto tag_opt = cli.add_option("--tag", dep_tag, "The github tag or commit hash; only valid when LOCATION is a github repository.");
   //auto commit_opt = cli.add_option("--commit", dep_commit, "The github tag or commit hash; only valid when LOCATION is a github repository.");
   //auto branch_opt = cli.add_option("--branch", dep_branch, "The github tag or commit hash; only valid when LOCATION is a github repository.");
   cli.add_option("--rel,--release", dep_rel, "A github release version.")->excludes(tag_opt);
   cli.add_option("--hash", dep_hash, "SHA256 hash; only valid when LOCATION gets an archive (i.e. *.tar.gz or similar).");
   // Option flag
   cli.add_flag("--interactive", interactive, "Force interactive mode.");

   // Parse
   CLI11_PARSE(cli,argc,argv);


   // Load the project or exit.
   auto proj = load_project_or_exit(cli,path);


   // Done with CLI, evaluate interactive mode.  Assuming interactive mode if dependency name was empty.
   interactive |= dep_name.empty();       // interactive mode should be const from this point forward.
   for (const auto loop=interactive; loop;) {

      if (!obj_name.empty() && !dep_name.empty() && antler::project::dependency::validate_location(dep_loc, dep_tag, dep_rel, dep_hash)) {
         // Get the object to operate on.
         auto obj_vec = proj.object(obj_name);

         // If it doesn't exist, none of the existing values can be correct, so alert and jump straigt to individual queries.
         if (obj_vec.empty())
            std::cerr << obj_name << " does not exist in project.\n";
         else {

            // We have values, so query the user if they want to apply.
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
            auto obj = obj_vec[0];
            if (!dep_name.empty() && obj.dependency_exists(dep_name))
               std::cerr << dep_name << " already exists for " << obj_name << " in project.\n";

            if (is_this_correct()) // Correct, then break out to sanity check and apply!
               break;
         }
      }

      // This container is used for validation.
      std::vector<antler::project::object> obj_vec;

      // Here we want to ensure that object exists.
      {
         auto validator = [&proj,&obj_vec](std::string_view s) {
            if (!validate_name(s))
               return false;
            if (obj_vec = proj.object(s); obj_vec.empty()) {
               std::cerr << s << " does not exist in " << proj.name() << '\n';
               return false;
            }
            return true;
         };
         get_valid_string("object (app/lib/test) name", obj_name, validator);
      }

      // Here we want to validate dep name does NOT already exist before we go on.
      {
         auto validator = [&obj_vec,&obj_name](std::string_view s) {
            if (!validate_name(s))
               return false;
            bool rv = false;
            for (const auto& a: obj_vec) {
               rv |= !a.dependency_exists(s);
            }
            if(!rv)
              std::cerr << s << " already exists for " << obj_name << " in project.\n";
            return rv;
         };
         get_valid_string("dependency name", dep_name, validator);
      }

      get_loc("from/location", dep_loc, true);
      get_name("git tag/commit hash", dep_tag, true);
      get_name("git release version", dep_rel, true);
      get_hash("SHA-256 hash", dep_hash, true);
   }


   // Done with interactive mode, validate and store data.


   // Get the object to update.
   auto obj_vec = proj.object(obj_name);
   if (obj_vec.empty())
      return cli.exit( CLI::Error("", obj_name + " does not exist in project.") );
   auto obj = obj_vec[0];

   // If we are not in interactive mode, test for the pre-existence of the dependency.
   if (!interactive && obj.dependency_exists(dep_name))
      return cli.exit( CLI::Error("",  dep_name + " already exists for " + obj_name + " in project.") );

   // Validate the location. Redundant for interactive mode, but cheap in human time.
   std::ostringstream ss;
   if (!antler::project::dependency::validate_location(dep_loc, dep_tag, dep_rel, dep_hash, ss))
      return cli.exit( CLI::Error("", ss.str()) );

   // Create the dependency, store it in the object, and store the object in the project.
   antler::project::dependency dep;
   dep.set(dep_name, dep_loc, dep_tag, dep_rel, dep_hash);
   obj.upsert_dependency(std::move(dep));
   proj.upsert(std::move(obj));

   // Sync the project to storage.
   if (!proj.sync())
      return cli.exit( CLI::Error("", "failed to write project file.") );
   return 0;
}
