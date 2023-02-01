/// @copyright See `LICENSE` in the root directory of this project.

#include <iostream>
#include <filesystem>
#include <ctype.h>
#include <sstream>
#include <vector>

#include <antler/project/project.h>
#include <aproj-common.h>



constexpr std::string_view brief_str = "Initialize a new projet creating the directory tree and a `project.yaml` file.";
std::string exe_name;
std::string indirect;

/// Print usage information to std::cout and return 0 or, optionally - if err is not-empty() - print to std::cerr and return -1.
/// @param err  An error string to print. If empty, print to std::cout and return 0; otherwise std::cerr and return -1.
/// @return 0 if err.empty(); otherwise -1. This value is suitable for using at exit.
int usage(std::string_view err) {

   std::ostream& os = (err.empty() ? std::cout : std::cerr);

   os << exe_name << ": PATH [PROJECT_NAME [VERSION]]\n"
      << "  " << brief_str << '\n'
      << '\n'
      << " PATH is the root path to create the project in.\n"
      << " PROJECT_NAME is the the name of the project.\n"
      << " VERSION is the version to store in the project file.\n"
      << '\n'
      << " `project.yaml` is created in PATH if PATH is an empty directory AND the filename matches PROJECT_NAME;\n"
      << "  otherwise, a directory matching PROJECT_NAME is created at PATH to contain `project.yaml`.\n"
      << '\n'
      << " If PROJECT_NAME is absent, the user is prompted.\n"
      << " If PROJECT_NAME exists, VERSION will default to 0.0.0\n"
      << '\n';

   if (err.empty())
      return 0;
   os << "Error: " << err << '\n';
   return -1;
}


int main(int argc, char** argv) {

   COMMON_INIT;

   if (argc < 2)
      return usage("path is required.");
   if (argc > 4)
      return usage("too many options.");

   std::error_code sec;

   std::string name;

   // Sanity check potential project directory.
   auto in_path = std::filesystem::path(argv[1]);
   if (!std::filesystem::exists(in_path, sec))
      name = in_path.filename().string();
   else {
      // It might be okay if it exists, but only if it's a directory AND it's empty.
      if (!std::filesystem::is_directory(in_path, sec))
         return usage(in_path.string() + " already exists.");
      if (!std::filesystem::is_empty(in_path, sec)) {
         if (std::filesystem::exists(in_path / "project.yaml"))
            return usage("not initializing where a `project.yaml` file already exists.");
      } else if (!in_path.has_extension())
         name = in_path.filename().string();
   }

   antler::project::version ver("0.0.0");

   if (argc > 2)
      name = argv[2];
   if (argc > 3)
      ver = argv[3];

   auto project_root = in_path;
   if (in_path.filename() != name)
      project_root /= name;

   if (argc == 2) {
      for (;;) {
         if (!name.empty()) {

            project_root = in_path;
            if (in_path.filename() != name)
               project_root /= name;

            std::cout
               << '\n'
               << "Path:         " << project_root << '\n'
               << "Project name: " << name << '\n'
               << "Version:      " << ver << '\n'
               << '\n';

            if (is_this_correct())
               break;
         }

         for (;;) {
            std::cout << "Enter project name: [" << name << "]" << std::flush;
            std::string temp;
            std::getline(std::cin, temp);
            if (is_valid_name(temp))
               name = temp;
            if (!name.empty())
               break;
         }

         {
            std::cout << "Enter project version: [" << ver << "]" << std::flush;
            std::string temp;
            std::getline(std::cin, temp);
            if (!temp.empty())
               ver = temp;
         }
      }
   }


   if (!is_valid_name(name))
      RETURN_USAGE(<< "name \"" << name << "\" contains invalid chars. Expecting [0-9a-zA-Z_].");


   // Do initialization here:

   // Create the root directory.
   std::filesystem::create_directories(project_root, sec);
   if (sec)
      RETURN_USAGE(<< project_root << " could not be created: " << sec);


   if (!std::filesystem::is_empty(project_root, sec))
      RETURN_USAGE(<< project_root << " is NOT empty!");

   // Create the directory structure.
   {
      const std::vector<std::filesystem::path> files = { "apps", "include", "ricardian", "libs", "tests" };
      for (const auto& fn : files) {
         std::filesystem::create_directory(project_root / fn, sec);
         if (sec)
            RETURN_USAGE(<< (project_root / fn) << " could not be created: " << sec);
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
