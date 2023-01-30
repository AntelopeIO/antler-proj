/// @copyright See `LICENSE` in the root directory of this project.

#include <iostream>
#include <filesystem>

#include <antler/project/project.h>

#include <aproj-common.h>



std::string exe_name;
std::string indirect;


/// Print usage information to std::cout and return 0 or, optionally - if err is not-empty() - print to std::cerr and return -1.
/// @param err  An error string to print. If empty, print to std::cout and return 0; otherwise std::cerr and return -1.
/// @return 0 if err.empty(); otherwise -1. This value is suitable for using at exit.
int usage(std::string_view err) {

   std::ostream& os = (err.empty() ? std::cout : std::cerr);

   os << exe_name << ": PATH [APP_NAME [APP_LANG [APP_OPTIONS]]]\n"
      << "\n"
      << "  --help         Print this help and exit.\n"
      << "\n"
      << " PATH is either path to `project.yaml` or the path containing it.\n"
      << " APP_NAME is the the name of the app to add.\n"
      << " APP_LANG is the language of the additional app.\n"
      << " APP_OPTIONS is the string of options to pass to the compiler.\n"
      << "\n"
      << " `project.yaml` is updated to add a new app.\n"
      << "\n"
      << " If either APP_NAME or APP_LANG is absent, the user is prompted.\n"
      << "\n";

   if (err.empty())
      return 0;
   os << "Error: " << err << "\n";
   return -1;
}


int main(int argc, char** argv) {

   COMMON_INIT("Populate the project with CMake files.");

   // Test arg count is valid.
   if (argc < 2)
      return usage("path is required.");
   if (argc > 2)
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

   auto pop_type = antler::project::project::pop::honor_deltas;

   bool result = proj.populate(pop_type);

   if (result)
      return 0;
   std::cerr << "Fail\n";
   return -1;
}
