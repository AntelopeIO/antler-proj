/// @copyright See `LICENSE` in the root directory of this project.

#include <iostream>
#include <filesystem>
#include <CLI11.hpp>

#include <antler/project/project.hpp>

#include <aproj-common.hpp>


int main(int argc, char** argv) {

   common_init(argc,argv,"Populate the project with Build files and dependencies.");

   std::filesystem::path path;

   CLI::App cli(brief_str,exe_name);

   // Positional arguments:
   cli.add_option("path", path, "This must be the path to `project.yaml` or the path containing it.")->required();

      // Parse
   CLI11_PARSE(cli,argc,argv);

   // Get the path to the project.
   if (!antler::project::project::update_path(path))
      return cli.exit( CLI::Error("path","path either did not exist or no `project.yaml` file could be found.") );

   // Load the project.
   auto optional_proj = antler::project::project::parse(path);
   if (!optional_proj)
      return cli.exit( CLI::Error("path","Failed to load project file.") );
   auto proj = optional_proj.value();

   auto pop_type = antler::project::project::pop::honor_deltas;

   bool result = proj.populate(pop_type);

   if (result)
      return 0;
   std::cerr << "Fail\n";
   return -1;
}
