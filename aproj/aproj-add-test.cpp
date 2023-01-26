// aproj-add-app.cpp and aproj-add-lib.cpp are nearly identical while aproj-add-test.cpp has a few differences.  If you change any
// of the three, make sure to keep the others similarly updated.

#include <iostream>
#include <filesystem>
#include <ctype.h>
#include <sstream>
#include <vector>

#include <antler/project/project.h>

#include <aproj-common.h>



std::string exe_name;
std::string indirect;

int usage(std::string_view err) {

   std::ostream& os = (err.empty() ? std::cout : std::cerr);

   os << exe_name << ": PATH [TEST_NAME [TEST_CMD]]\n"
      << "\n"
      << " PATH is either path to `project.yaml` or the path containing it.\n"
      << " TEST_NAME is the the name of the test to add.\n"
      << " TEST_CMD is the testing command to execute.\n"
      << "\n"
      << " `project.yaml` is updated to add a new test.\n"
      << "\n"
      << " If either TEST_NAME or TEST_CMD is absent, the user is prompted.\n"
      << " Note that an empty value for TEST_CMD is valid (e.g. \"\" is a valid empty argument.\n"
      << "\n"
      ;

   if(err.empty())
      return 0;
   os << "Error: " << err << "\n";
   return -1;
}


int main(int argc, char** argv) {

   COMMON_INIT("Add a test entry.");

   // Test arg count is valid.
   if(argc < 2)
      return usage("path is required.");
   if(argc > 4)
      return usage("too many options.");

   // Get the path to the project.
   std::filesystem::path path=argv[1];
   if(!antler::project::project::update_path(path))
      return usage("path either did not exist or no `project.yaml` file could be found.");

   // Load the project.
   auto optional_proj = antler::project::project::parse(path);
   if( !optional_proj )
      return usage("Failed to load project file.");
   auto proj = optional_proj.value();


   std::string name;
   std::string cmd;

   if(argc >= 3) {
      name = argv[2];
      if(proj.object_exists(name, antler::project::object::type_t::test))
         return usage("TEST_NAME already exists in project.");
   }

   if(argc >= 4)
      cmd = argv[3];
   else {
      for(;;) {
         if(!name.empty()) {

            std::cout
               << "\n"
               << "test name: " << name << "\n"
               << "command:   " << cmd << "\n"
               << "\n"
               ;

            if(proj.object_exists(name, antler::project::object::type_t::test)) {
               std::cerr << "Test " << name << " already exists in project. Can't add.\n\n";
            }
            else {
               if(proj.object_exists(name))
                  std::cerr << "WARNING: " << name << " already exists in project as app and/or lib.\n\n";

               if(is_this_correct())
                  break;
            }
         }

         get_name("test name", name);

         for(;;) {
            std::cout << "Enter test command (space to clear): [" << cmd << "]" << std::flush;
            std::string temp;
            std::getline(std::cin,temp);
            if(temp == " ")
               cmd.clear();
            else if(!temp.empty())
               cmd = temp;
            else
               continue;
            break;
         }
      }
   }


   auto obj = antler::project::object(name, cmd);
   proj.upsert_test(std::move(obj));
   proj.sync();

   return 0;
}
