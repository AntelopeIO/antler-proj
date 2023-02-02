/// @copyright See `LICENSE` in the root directory of this project.

// aproj-add-app.cpp and aproj-add-lib.cpp are nearly identical while aproj-add-test.cpp has a few differences.  If you change any
// of the three, make sure to keep the others similarly updated.

#include <iostream>
#include <filesystem>
#include <ctype.h>
#include <sstream>
#include <vector>

#include <antler/project/project.h>

#include <aproj-common.h>


/// Print usage information to std::cout and return 0 or, optionally - if err is not-empty() - print to std::cerr and return -1.
/// @param err  An error string to print. If empty, print to std::cout and return 0; otherwise std::cerr and return -1.
/// @return 0 if err.empty(); otherwise -1. This value is suitable for using at exit.
int usage(std::string_view err) {

   std::ostream& os = (err.empty() ? std::cout : std::cerr);

   os << exe_name << ": PATH [LIB_NAME [LIB_LANG [LIB_OPTIONS]]]\n"
      << "  " << brief_str << '\n'
      << '\n'
      << " PATH is either path to `project.yaml` or the path containing it.\n"
      << " LIB_NAME is the the name of the lib to add.\n"
      << " LIB_LANG is the language of the additional lib.\n"
      << " LIB_OPTIONS is the string of options to pass to the compiler.\n"
      << '\n'
      << " `project.yaml` is updated to add a new lib.\n"
      << '\n'
      << " If either LIB_NAME or LIB_LANG is absent, the user is prompted.\n"
      << '\n';

   if (err.empty())
      return 0;
   os << "Error: " << err << '\n';
   return -1;
}


int main(int argc, char** argv) {

   common_init(argc,argv,"Add a library entry.");

   // Test arg count is valid.
   if (argc < 2)
      return usage("path is required.");
   if (argc > 5)
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
   antler::project::language lang = antler::project::language::none;
   std::string opts;

   if (argc >= 3) {
      name = argv[2];
      if (proj.object_exists(name, antler::project::object::type_t::lib))
         return usage("LIB_NAME already exists in project.");
   }

   if (argc == 5)
      opts = argv[3];

   if (argc >= 4)
      lang = antler::project::to_language(argv[3]);
   else {
      for (;;) {
         if (!name.empty() && lang != antler::project::language::none) {

            std::cout
               << '\n'
               << "lib name: " << name << '\n'
               << "language: " << lang << '\n'
               << "options:  " << opts << '\n'
               << '\n';

            if (proj.object_exists(name, antler::project::object::type_t::lib)) {
               std::cerr << "Library " << name << " already exists in project. Can't add.\n\n";
            } else {
               if (proj.object_exists(name))
                  std::cerr << "WARNING: " << name << " already exists in project as app and/or test.\n\n";

               if (is_this_correct())
                  break;
            }
         }

         get_name("library name", name);

         for (;;) {
            std::cout << "Enter project language: [" << lang << "]" << std::flush;
            std::string temp;
            std::getline(std::cin, temp);
            if (temp.empty() && lang != antler::project::language::none)
               break;
            antler::project::language l2 = antler::project::to_language(temp);
            if (l2 != antler::project::language::none) {
               lang = l2;
               break;
            }
         }

         {
            std::cout << "Enter library options (space to clear): [" << opts << "]" << std::flush;
            std::string temp;
            std::getline(std::cin, temp);
            if (temp == " ")
               opts.clear();
            else if (!temp.empty())
               opts = temp;
         }
      }
   }


   if (lang == antler::project::language::none)
      return usage("invalid language.");
   auto obj = antler::project::object(antler::project::object::lib, name, lang, opts);
   proj.upsert_lib(std::move(obj));
   proj.sync();

   return 0;
}
