/// @copyright See `LICENSE` in the root directory of this project.

#include <iostream>
#include <filesystem>
#include <antler/project/project.hpp>
#include <CLI11.hpp>

#include <aproj-common.hpp>


int main(int argc, char** argv) {

   common_init(argc,argv,"Attempt to load a project.yaml file. This is the default command.");

   std::filesystem::path path;
   bool quiet = false;

   CLI::App cli(brief_str,exe_name);

   // Positional arguments:
   cli.add_option("path", path, "This must be the path to `project.yaml` or the path containing it.")->required();


   // Option flag
   cli.add_flag("--quiet,-q", quiet, "Do NOT print the contents of the project.yaml file.");

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

   if (!quiet)
      std::cout << proj.to_yaml() << '\n';

   return 0;
}
