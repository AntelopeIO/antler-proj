#include <iostream>
#include <filesystem>
#include <aproj-common.h>
#include <antler/project/project.h>

std::string exe_name;
std::string indirect;

int usage(std::string_view err) {

   std::ostream& os = (err.empty() ? std::cout : std::cerr);

   os << exe_name << ": PATH\n"
      << "\n"
      << " Attempt to load the project.yaml file pointed to by PATH.\n"
      << "\n"
      << "  --help         Print this help and exit.\n"
      << "  -q,--quiet     Do NOT print the contents of the project.yaml file.\n"
      << "\n"
      << " PATH is either path to `project.yaml` or the path containing it.\n"
      << "\n"
      ;

   if(err.empty())
      return 0;
   os << "Error: " << err << "\n";
   return -1;
}


int main(int argc, char** argv) {

   COMMON_INIT("Attempt to load a project.yaml file. This is the default command.");

   // Test arg count is valid.
   if(argc < 2)
      return usage("path is required.");
   if(argc > 3)
      return usage("too many options.");


   // Get the path to the project.
   std::filesystem::path path=argv[1];
   if(!antler::project::project::update_path(path))
      return usage("path either did not exist or no `project.yaml` file could be found.");

   bool quiet=false;

   for(int i=2; i < argc; ++i) {
      std::string_view arg = argv[i];
      if( arg == "-q" || arg == "--quiet") {
         quiet = true;
         continue;
      }

      RETURN_USAGE( << "argument " << arg << " not valid in this context.");
   }

   // Load the project.
   auto optional_proj = antler::project::project::parse(path);
   if( !optional_proj )
      return usage("Failed to load project file.");
   auto proj = optional_proj.value();

   if(!quiet)
      std::cout << proj.to_yaml() << "\n";

   return 0;
}
