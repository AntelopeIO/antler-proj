/// @copyright See `LICENSE` in the root directory of this project.

// aproj-add-app.cpp and aproj-add-lib.cpp are nearly identical while aproj-add-test.cpp has a few differences.  If you change any
// of the three, make sure to keep the others similarly updated.


#include <iostream>
#include <filesystem>
#include <vector>
#include <CLI11.hpp>

#include <antler/project/project.hpp>

#include <aproj-common.hpp>


int main(int argc, char** argv) {

   common_init(argc,argv,"Add an application entry to the project.");

   std::filesystem::path path;
   std::string name;
   antler::project::language lang = antler::project::language::none;
   std::string opts;
   bool interactive=false;


   // CLI set up.

   CLI::App cli(brief_str,exe_name);

   // Positional arguments:
   cli.add_option("path", path, "This must be the path to `project.yaml` or the path containing it.")->required();
   cli.add_option("app_name", name, "The name of the application to add.");
   cli.add_option("app_lang", lang, "The application's language (e.g. `cpp`, `java`, `c`, etc.).");
   cli.add_option("app_options", opts, "Options to pass to the compiler.");

   // Option flag
   cli.add_flag("--interactive", interactive, "Force interactive mode.");

   // Parse
   CLI11_PARSE(cli,argc,argv);


   // Load the project or exit.
   auto proj = load_project_or_exit(cli, path);


   // Interactive mode?
   interactive |= (lang == antler::project::language::none);
   for (const auto loop=interactive; loop;) {
      // Loop until the user says values are correct.
      // Only query for correct if all the info is updated.
      if (!name.empty() && lang != antler::project::language::none) {

         std::cout
            << '\n'
            << "app name: " << name << '\n'
            << "language: " << lang << '\n'
            << "options:  " << opts << '\n'
            << '\n';

         if (proj.object_exists(name, antler::project::object::type_t::app)) {
            std::cerr << "Application " << name << " already exists in project. Can't add.\n\n";
         }
         else {
            if (proj.object_exists(name))
               std::cerr << "WARNING: " << name << " already exists in project as lib and/or test.\n\n";

            if (is_this_correct())
               break;
         }
      }

      get_name("application name", name);
      get_language("project language", lang);
      get_string("application options", opts, true);
   }


   // Sanity check and apply.
   if (!name.empty() && proj.object_exists(name, antler::project::object::type_t::app))
      return cli.exit( CLI::Error("name", "app_name already exists in project.") );
   if (lang == antler::project::language::none)
      return cli.exit( CLI::Error("name", "invalid language.") );

   auto obj = antler::project::object(antler::project::object::app, name, lang, opts);
   proj.upsert_app(std::move(obj));
   proj.sync();

   return 0;
}
