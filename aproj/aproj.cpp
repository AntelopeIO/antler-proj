/// @copyright See `LICENSE` in the root directory of this project.

#include <iostream>
#include <string>
#include <string_view>
#include <filesystem>
#include <vector>
#include <optional>
#include <span>

#include <boost/algorithm/string.hpp>        // boost::split()
#include <boost/dll/runtime_symbol_info.hpp> // boost::dll::program_location()

#include <antler/system/exec.hpp>


#include <aproj-prefix.hpp>


namespace { // anonymous

// The executable name as derived from argv[0] for display in --help command.
std::string exe_name;

/// Structure to store contents of subcommands/flags
struct app_entry {
   std::filesystem::path path;     ///< path to the executable subcommand.
   std::string raw_flags;          ///< Raw string representation of args (e.g. "-q, --quiet"). This is used to calculate pretty column widths.
   std::vector<std::string> flags; ///< flags to select this subcomand (e.g. {"-q", "--quiet"}).
   std::string brief;              ///< Brief description adequate for displaying what the subcommand does when `--help` is called.

   /// Test to see if this structure has a given command (cmd).
   /// @param cmd  The command to search for.
   /// @return true if cmd was found, false otherwise.
   bool has_command(std::string_view cmd) const noexcept {
      for (const auto& flag : flags) {
         if (flag == cmd)
            return true;
      }
      return false;
   }

#if _DEBUG
   void print(std::ostream& os) const noexcept {
      os << path << " flags: [";
      for(size_t i=0; i < flags.size(); ++i) {
         if(i)
            os << ",";
         os << flags[i];
      }
      os << "] raw: [" << raw_flags << "] brief: " << brief << "\n";
   }
#endif
};
// Storage for subcommands.
std::vector<app_entry> apps;


/// Print usage information to std::cout and return 0 or, optionally - if err is not-empty() - print to std::cerr and return -1.
/// @param err  An error string to print. If empty, print to std::cout and return 0; otherwise std::cerr and return -1.
/// @return 0 if err.empty(); otherwise -1. This value is suitable for using at exit.
int usage(std::string_view err = "") {

   constexpr std::string_view help_arg{ "--help" };

   // Determine how wide the flags "column" should be.
   size_t width = help_arg.size();
   for (const auto& a : apps) {
      width = std::max(width, a.raw_flags.size());
   }
   width += 3;

   // Sort the subcommands based on flag value.
   std::sort(apps.begin(), apps.end(), [](const app_entry& l, const app_entry& r) { return l.raw_flags < r.raw_flags; });

   // Determine target ostream.
   std::ostream& os = (err.empty() ? std::cout : std::cerr);

   os << exe_name << ": COMMAND [options]\n"
      << "\n"
      << " Commands:\n";

   // Print the flags followed by brief for each subcommand.
   for (const auto& a : apps) {
      // Make sure to pad for a nice column.
      std::string pad(width - a.raw_flags.size(), ' ');
      os << "  " << a.raw_flags << pad << a.brief << '\n';
   }

   // add --help arg:
   {
      std::string pad(width - help_arg.size(), ' ');
      os << "  " << help_arg << pad << "Show this help and exit.\n";
   }

   os << '\n'
      << " Options vary by command and may be viewed with <command> --help.\n";

   // Return success(0) or print the error and return failure(-1).
   if (err.empty())
      return 0;
   os << "Error: " << err << "\n";
   return -1;
}


/// Helper function to limit boilerplate cut and paste when calling subcommands.
/// This function calls a subcommand and passes a string for use in the subcommands `usage()` function.
/// @tparam iterator_type  Nominally std::vector<std::string_view>::iterator, but could be const char*
/// @param exe  The path for the subcommand.
/// @param cmd  The flag/command used to indicate which exe to call.
/// @param begin  Iterator to the beginning of a list of arguments that will be passed.
/// @param end  Iterator to the end of a list of arguments that will be passed.
/// @return The result of calling exe with the given arguments.
template<typename iterator_type>
int exec_helper(const std::filesystem::path& exe, std::string_view cmd, iterator_type begin, iterator_type end) {

   std::stringstream ss;
   ss << exe;
   // Pass all the arguments.
   for (auto i = begin; i < end; ++i)
      ss << " " << *i;
   // Add the string for use in the subcomand's usage() function.
   ss << " --indirect=\"" << exe_name << ' ' << cmd << '"';

   // Call the subcommand.
   return system(ss.str().c_str());
}


/// Search the directory containing this executable for its subcommands and return them to the caller.
/// @return A vector containing the absolute paths to the subcommands.
std::vector<std::filesystem::path> get_aproj_subcommands() {

   std::vector<std::filesystem::path> rv; // return value.

   // As of this writting, program location returns a boost::dll::path type that must be explicityly converted to a std::filesystem::path.
   const std::filesystem::path bin_path = boost::dll::program_location().parent_path().string();  // This could throw, but really shouldn't.

   // Iterate over the collocated executables, if they match our pattern store them in the return value list.
   for (auto const& entry : std::filesystem::directory_iterator{ bin_path }) {
      const auto& path = entry.path();
      if (path.stem().string().starts_with(project_prefix))   // project_prefix should be "aproj-".
         rv.push_back(path);
   }

   return rv;
}


/// Populate app list
/// Given an executable path, call it with the `--brief` falg and store the results in the app list.
/// @param path  Path to an `aproj-` subcommand executable.
/// @post The `app` global is updated with the results from calling `path` with the command `--brief`.
void populate_apps(const std::filesystem::path& path) noexcept {

   // Example output from `aproj-init --brief`:  "--init Initialize a new projet creating the directory tree and a `project.yaml` file."
   //   path:  "/usr/local/bin/aproj-init"
   //   flags: "--init"
   //   brief: "Initialize a new projet creating the directory tree and a `project.yaml` file."
   // Note: flags are parsed from the beginiing to the first space, so if the output was "-i,--init Initialize..." flags would have been "-i","--init"


   // Get the brief description from the subcommand executable:
   auto result = antler::system::exec(path.string() + " --brief");

   // On error: print the error and return.
   if (!result) {
      std::cerr << exe_name << ": `--brief` capture failed for " << path << '\n';
      return;
   }

   // Search for the spc to break the string into flags and brief.
   auto spc = result.output.find_first_of(' ');

   // If the space is missing report the error and return.
   if (spc == std::string::npos) {
      std::cerr << exe_name << ": `--brief` parse failed for " << path << '\n';
      return;
   }

   // Build an app entry from the path and results of capturing the brief output.
   app_entry ae;
   ae.path = path;
   ae.raw_flags = result.output.substr(0, spc);
   // Split the raw flags into a vector of strings. e.g. "-i,--init" becomes {"-i","--init"}
   boost::split(ae.flags, ae.raw_flags, boost::is_any_of(","));
   // Store the help text, then trim any new line characters.
   ae.brief = result.output.substr(spc + 1);
   while (ae.brief.back() == '\n')
      ae.brief.pop_back();

   // Finally, store the app_entry into the global list.
   apps.push_back(ae);
}


/// Search for an app entry that has either cmd or --cmd in it's flags.
/// @note if cmd is not found and does not begin with '-', "--" is appended and the search is rerun.
/// @param cmd  The command to search for.
/// @return A std::option that is populated with the app_entry ONLY if it is found.
[[nodiscard]] std::optional<app_entry> find_app_entry(const std::string& cmd) noexcept {

   // sanity check.
   if (cmd.empty())
      return {};

   // Look for the command in the apps.
   for (const auto& a : apps) {
      if (a.has_command(cmd))
         return a;
   }

   // If cmd starts with '-', we return. If not, we will add "--" to command and recheck.
   if (cmd[0] == '-')
      return {};

   // Look for the dashed command in the apps.
   auto dashed_cmd = std::string("--") + cmd;
   for (const auto& a : apps) {
      if (a.has_command(dashed_cmd))
         return a;
   }

   // No app was found. Return expty.
   return {};
}


/// Parse command. Attempt to convert argv values into a command and options.
/// @param begin  At entry, this should be the first argument (argv[1]) at exit this is the first option after the parsed command.
/// @param end  End iterator.
/// @param cmd  As long as begin != end, this is updated with the parsed command.
/// @param ae  If command was found
/// @return true if a command was found with an associated ae and begin, command, and ae are updated.
template <typename iterator_type>
[[nodiscard]] bool parse_command(iterator_type& begin, const iterator_type end, std::string& cmd, app_entry& ae) noexcept {

   if (begin == end)
      return false;

   // Get the command.
   cmd = *begin;
   ++begin;

   // See if the command was found, if it is, copy ae and return true.
   if (auto ae_opt = find_app_entry(cmd); ae_opt) {
      ae = ae_opt.value();
      return true;
   }

   // We didn't find the command, maybe it's special? If it's definately NOT, return false.
   if (begin == end)
      return false;

   // If this is the add command it needs a prefix.
   if (cmd == "add") {

      // Update command to add the prefix. eg "add" becomes "add app" or "add lib" or etc.
      // We don't expect to find "add X" in apps, but we should find "--add-X", so make a special alias  search command.
      cmd += std::string(" ") + std::string(*begin);
      auto alias_cmd = std::string("--add-") + *begin;
      ++begin;

      // See if we can find the alias command.
      if (auto ae_opt = find_app_entry(alias_cmd); ae_opt) {
         ae = ae_opt.value();
         return true;
      }

      // We didn't find the alias_cmd, return false.
      return false;
   }

   // If we got here, then no app_entry was found. Return false.
   return false;
}


} // anonymous namespace



int main(int argc, char** argv) {

   // Get the executable name, this is used for populating `--help` / usage() output.
   exe_name = std::filesystem::path(argv[0]).filename().string();

   // Get all the subcomands.
   auto subcommand_path_list = get_aproj_subcommands();

   // Populate the apps global list for each subcommand.
   for(const auto& subcommand_path : subcommand_path_list)
      populate_apps(subcommand_path);


   // Try try to find the command.
   auto span = std::span(argv+1, argc-1);
   auto args_begin = span.begin();
   auto args_end = span.end();
   std::string cmd;
   app_entry ae;
   if (parse_command(args_begin, args_end, cmd, ae)) {
      // We found the command, so let's call it!
      // Note: args_begin was updated by parse_command.
      return exec_helper(ae.path, cmd, args_begin, args_end);
   }

   // If cmd is empty, then no command was given, so provide the usage to stderr and exit.
   if (cmd.empty())
      return usage("No command supplied.");

   // The command did not result in finding an app_entry. If the command was `--help`, call usage.
   if (cmd == "--help")
      return usage();

   // The command was populated and NOT --help, call usage with the error.
   return usage(cmd + " is not a valid command.");
}
