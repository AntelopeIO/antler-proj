/// @copyright See `LICENSE` in the root directory of this project.

#include <iostream>
#include <string>
#include <string_view>
#include <filesystem>
#include <vector>
#include <CLI11.hpp>

#include <boost/algorithm/string.hpp>        // boost::split()
#include <boost/dll/runtime_symbol_info.hpp> // boost::dll::program_location()

#include <antler/system/exec.hpp>


#include <aproj-prefix.hpp>


namespace { // anonymous

std::string exe_name;
const std::string original_tag="<original tag>";

/// Helper function to limit boilerplate cut and paste when calling subcommands.
/// This function calls a subcommand and passes a string for use in the subcommands `usage()` function.
/// @param exe  The path for the subcommand.
/// @param cmd  The flag/command used to indicate which exe to call.
/// @param args  The arguments to pass
/// @return The result of calling exe with the given arguments.
[[nodiscard]] int exec_helper(std::filesystem::path exe, std::string cmd, std::vector<std::string>& args) {

   // If we overwrote the tag, we need to get it back. It will be the 1st (zero-ith) item in args.
   if(!args.empty() && args[0].starts_with(original_tag)) {
      // Copy the original arg into cmd and erase it from the vector.
      cmd=args[0].substr(original_tag.size());
      args.erase(args.begin());
   }

   // Create a string of the arguments.
   std::stringstream ss;
   ss << exe;
   // Pass all the arguments.
   for (const auto& a : args)
      ss << " " << a;
   // Subcommands need to know how they were called. We pass this as the indirect argument. Build it here.
   ss << " --indirect=\"" << exe_name << ' ' << cmd << '"';

   // Call the subcommand.
   return system(ss.str().c_str());
}

}


int main(int argc, char** argv) {

   int rv = 0;

   // Figure out various paths.
   const std::filesystem::path exe_name = std::filesystem::path(argv[0]).filename().string();
   const std::filesystem::path bin_path = boost::dll::program_location().parent_path().string();  // This could throw.

   // Massage argv.
   //   Because we want to allow for "aproj add app <options>" to equal "aproj --add-app <options>" we need to rewrite argv. In
   //   order to do this we need a way to store the new strings. We are going to do that here.
   std::vector<std::string> string_store;  // Holds strings for us, but musn't be resized!
   string_store.reserve(argc);             // Reserve to ensure resize is avoided.
   // Iterate over the args. Sample conversion {"add", "app"} into {"--add-app", "<original tag>add app"}.
   for(int i=1; i < (argc-1); ++i) {
      std::string orig_arg = argv[i];
      if(orig_arg == "add" || orig_arg == "rm") {
         // Convert `argv[i] argv[i+1]` into `--argv[i]-argv[i+1]` and store at argv[i].
         string_store.emplace_back(std::string("--") + orig_arg + "-" + argv[i+1]);
         argv[i] = string_store.back().data();
         // Store original args in argv[i+1] with a unique tag: "<original tag> argv[i] argv[i+1]".
         string_store.emplace_back(original_tag + orig_arg + " " + argv[i+1]);
         argv[i+1] = string_store.back().data();
         ++i; // Skip over argv[i+1]. This is an optimization.
      }
   }


   // This is a workaround for dynamically building the CLI parser. Options expect a vector, so we need a place to store
   // them. Since we will take the address of the vector in a lambda, we musn't allow the vector to be resized.
   std::vector<std::vector<std::string>> parsed_options; // Musn't be resized!
   parsed_options.reserve(argc);                         // Reserve to ensure resize is avoided.


   // Using CLI11 for command line parsing.
   CLI::App cli("antler-proj",exe_name);

   // Get all the collocated sub commands. Search only in this executable's directory.
   for (auto const& entry : std::filesystem::directory_iterator{ bin_path }) {
      // If the path does not start with our prefix, then skip it.
      const auto& path = entry.path();
      if (!path.stem().string().starts_with(project_prefix))
         continue;
      // Call the command (with `--brief`) to capture its brief help info.
      auto result = antler::system::exec(path.string() + " --brief");
      if (!result) {            // Warn if an executable isn't reporting brief info.
         std::cerr << "Warning: Failed to get brief help for " << path << '\n';
      }
      else {
         // Parse out the command flag and its brief description. The separtion is a single space.
         auto spc = result.output.find_first_of(' ');
         if (spc != std::string::npos) {
            try {
               // Brief description must be "<cmd> <brief text>" cmd must be a valid CLI11::App::add_subcomand() command string.
               // Note: add_subcommand() is patched to allow the command to begin with `-`.
               std::string cmd = result.output.substr(0, spc);
               auto brief = result.output.substr(spc + 1);
               auto sc = cli.add_subcommand(cmd,brief,true);
               // Store a vector for collecting the option strings.
               parsed_options.emplace_back(std::vector<std::string>{});
               auto& v = parsed_options.back();
               // Add the "options" option with vector storage and "options" description.
               sc->add_option("options", v, "options");
               // Set the callback. It takes the executable path of the subcommand, the cmd itself, the options vector, and the return value.
               sc->final_callback(
                                  [path,cmd,&v,&rv](){
                                     int temp = exec_helper(path, cmd, v);
                                     if (temp != 0)
                                        rv = temp;
                                  }
                                  );
            }
            catch(std::runtime_error& e) {
               // Report an error if there was a problem adding the subcommand.
               std::cerr << "Error: Cant add " << path << " with brief \"" << result.output << "\", error: " << e.what() << '\n';
            }
         }
      }
   }

   // Parse the args. This will result in the calls
   CLI11_PARSE(cli,argc,argv);

   return rv;
}
