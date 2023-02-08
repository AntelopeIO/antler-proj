/// @copyright See `LICENSE` in the root directory of this project.

// aproj-add-app.cpp and aproj-add-lib.cpp are nearly identical while aproj-add-test.cpp has a few differences.  If you change any
// of the three, make sure to keep the others similarly updated.

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

   os << exe_name << ": PATH [TEST_NAME [TEST_CMD]]\n"
      << "  " << brief_str << '\n'
      << '\n'
      << " PATH is either path to `project.yaml` or the path containing it.\n"
      << " TEST_NAME is the the name of the test to add.\n"
      << " TEST_CMD is the testing command to execute.\n"
      << '\n'
      << " `project.yaml` is updated to add a new test.\n"
      << '\n'
      << " If either TEST_NAME or TEST_CMD is absent, the user is prompted.\n"
      << " Note that an empty value for TEST_CMD is valid (e.g. \"\" is a valid empty argument.\n"
      << '\n';

   if (err.empty())
      return 0;
   os << "Error: " << err << '\n';
   return -1;
}


int main(int argc, char** argv) {

   common_init(argc,argv,"Add a test entry.");

   // Test arg count is valid.
   if (argc < 2)
      return usage("path is required.");
   if (argc > 4)
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


   std::string name;
   std::string cmd;

   if (argc >= 3) {
      name = argv[2];
      if (proj.object_exists(name, antler::project::object::type_t::test))
         return usage("TEST_NAME already exists in project.");
   }

   // Interactive?
   if (argc >= 4) {
      // Non-interactive path.
      cmd = argv[3];
   }
   else {
      // Interactive path.
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
