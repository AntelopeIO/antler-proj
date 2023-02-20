/// @copyright See `LICENSE` in the root directory of this project.

#include <iostream>
#include <filesystem>
#include <CLI11.hpp>

#include <antler/project/project.hpp>

#include <aproj-common.hpp>


int main(int argc, char** argv) {

   common_init(argc,argv,"Populate the project with Build files and dependencies.");

   std::filesystem::path path;


   // CLI setup

   CLI::App cli(brief_str,exe_name);

   // Positional arguments:
   cli.add_option("path", path, "This must be the path to `project.yaml` or the path containing it.")->required();

   // Parse
   CLI11_PARSE(cli,argc,argv);


   // Load project or exit.
   auto proj = load_project_or_exit(cli,path);


   // do the work.

   auto pop_type = antler::project::project::pop::honor_deltas;

   bool result = proj.populate(pop_type);

   if (result)
      return 0;
   std::cerr << "Fail\n";
   return -1;
}
