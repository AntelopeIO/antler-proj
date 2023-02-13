/// @copyright See `LICENSE` in the root directory of this project.

// aproj-add-app.cpp and aproj-add-lib.cpp are nearly identical while aproj-add-test.cpp has a few differences.  If you change any
// of the three, make sure to keep the others similarly updated.


#include <iostream>
#include <filesystem>
#include <vector>
#include <CLI11.hpp>

#include <antler/project/project.hpp>

#include <aproj-common.hpp>


int main(int argc, char** argv) {

   common_init(argc,argv,"Add a test entry.");

   std::filesystem::path path;
   std::string name;
   std::string cmd;
   bool interactive;

   CLI::App cli(brief_str,exe_name);

   // Positional arguments:
   cli.add_option("path", path, "This must be the path to `project.yaml` or the path containing it.")->required();
   cli.add_option("test_name", name, "The name of the test to add.");
   cli.add_option("test_cmd", cmd, "The test command to execute.");

   // Option flag
   cli.add_flag("--interactive", interactive, "Force interactive mode.");

   // Parse
   CLI11_PARSE(cli,argc,argv);

   // Get the path to the project.
   if (!antler::project::project::update_path(path))
      return cli.exit( CLI::Error("path","path either did not exist or no `project.yaml` file could be found.") );

   // Load the project.
   auto optional_proj = antler::project::project::parse(path);
   if (!optional_proj)
      return cli.exit( CLI::Error("path", "Failed to load project file.") );
   auto proj = optional_proj.value();

   // Sanity checking
   if (!name.empty() && proj.object_exists(name, antler::project::object::type_t::test)) {
      return cli.exit( CLI::Error("name", "test_name already exists in project.") );
   }

   interactive |= cmd.empty();

   if (interactive) {
      // Loop until the user says values are correct.
      for (;;) {
         // If name is populated, we can show the info so far. If it's not populated, then skip straight to the queries.
         if (!name.empty()) {

            // Print values.
            std::cout
               << '\n'
               << "test name: " << name << '\n'
               << "command:   " << cmd << '\n'
               << '\n';

            // Check to see if name is a TEST duplicate.
            if (proj.object_exists(name, antler::project::object::type_t::test)) {
               // Enform user of the duplicate.
               std::cerr << "Test " << name << " already exists in project. Can't add.\n\n";
            } else {
               // Check to see if name is otherwise a duplicate, warn if so.
               if (proj.object_exists(name))
                  std::cerr << "WARNING: " << name << " already exists in project as app and/or lib.\n\n";
               // Ask if the printed values are correct, if so break out of this loop.
               if (is_this_correct())
                  break;
            }
         }

         // Querry for test name.
         get_name("test name", name);

         // Querry for test command.
         get_string("test command", cmd, true);
      }
   }


   auto obj = antler::project::object(name, cmd);
   proj.upsert_test(std::move(obj));
   proj.sync();

   return 0;
}
