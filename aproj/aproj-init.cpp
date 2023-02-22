/// @copyright See `LICENSE` in the root directory of this project.

#include <iostream>
#include <filesystem>
#include <vector>
#include <CLI11.hpp>

#include <antler/project/project.hpp>

#include <aproj-common.hpp>


int main(int argc, char** argv) {

   std::filesystem::path cli_path;
   std::string cli_name;
   antler::project::version ver("0.0.0");
   bool interactive=false;


   // CLI setup

   // Description needs a bit more info than our standard brief string contains; still, we cannot break brief_str.
   const std::string brief_str = "Initialize a new projet creating the directory tree and a `project.yaml` file.";

   CLI::App cli{};
   common_init(cli, argv[0], brief_str);

   const std::string desc=brief_str + "\n"
      + "`project.yaml` is created in PATH if PATH is an empty directory AND the filename matches PROJECT_NAME;\n"
      + "otherwise, a directory matching PROJECT_NAME is created at PATH to contain `project.yaml`.\n";
   cli.description(desc);

   // Positional arguments:
   cli.add_option("path", cli_path, "This is the root path to create the project in.")->required();
   cli.add_option("project_name", cli_name, "The name of the project.");
   cli.add_option("version", cli_name, "The version to store in the project file.");  // add default info?

   // Option flag
   cli.add_flag("--interactive", interactive, "Force interactive mode.");

   // Parse
   CLI11_PARSE(cli,argc,argv);


   // Begin doing the work. (This may move into `antler::project::project` at a later date.)

   // `name` will contain the project name. It needs to be separate from cli_name because we might use both in the logic below.
   std::string name;

   // Sanity check potential project directory.
   std::error_code sec;
   if (!std::filesystem::exists(cli_path, sec))
      name = cli_path.filename().string();
   else {
      // It might be okay if it exists, but only if it's a directory AND it's empty.
      if (!std::filesystem::is_directory(cli_path, sec))
         return cli.exit( CLI::Error("path", cli_path.string() + " already exists.") );
      if (!std::filesystem::is_empty(cli_path, sec)) {
         if (std::filesystem::exists(cli_path / "project.yaml"))
            return cli.exit( CLI::Error("path", "not initializing where a `project.yaml` file already exists.") );
      }
      else if (!cli_path.has_extension()) {
         name = cli_path.filename().string();
      }
   }

   // Maybe copy name from cli.
   if(!cli_name.empty())
      name = cli_name;

   // Resolve the path to the project root. This may be overwritten in interactive mode.
   auto project_root = cli_path;
   if (cli_path.filename() != name)
      project_root /= name;

   // Test for interactive mode.
   interactive |= cli_name.empty();
   for (const auto loop=interactive; loop;) {
      // Loop until user is satisfied.
      if (!name.empty()) {
         // Resolve the path to the project root.
         project_root = cli_path;
         if (cli_path.filename() != name)
            project_root /= name;

         std::cout
            << '\n'
            << "Path:         " << project_root << '\n'
            << "Project name: " << name << '\n'
            << "Version:      " << ver << '\n'
            << '\n';
         if(!ver.is_semver())
            std::cout << "Warning: Version is NOT a SemVer.\n\n";

         if (is_this_correct())
            break;
      }

      get_name("project name",name);
      get_version("project version", ver);
   }


   // Sanity check.

   if (!validate_name(name))
      return cli.exit( CLI::Error("name", std::string{"name \""} + name + "\" contains invalid chars. Expecting [0-9a-zA-Z_].") );


   // Do initialization here:

   // Create the root directory.
   std::filesystem::create_directories(project_root, sec);
   if (sec)
      return cli.exit( CLI::Error("path", std::string{project_root} + " could not be created: " + sec.message()) );


   if (!std::filesystem::is_empty(project_root, sec))
      return cli.exit( CLI::Error("path", std::string{project_root} + " is NOT empty!") );

   // Create the directory structure.
   {
      const std::vector<std::filesystem::path> files = { "apps", "include", "ricardian", "libs", "tests" };
      for (const auto& fn : files) {
         std::filesystem::create_directory(project_root / fn, sec);
         if (sec)
            return cli.exit( CLI::Error("path", std::string{project_root / fn} + " could not be created: " + sec.message()) );
      }
   }

   // Create an empty project and populate it.
   antler::project::project proj;
   proj.path(project_root / "project.yaml");
   proj.name(name);
   proj.version(ver);
   proj.sync();


   return -1;
}
