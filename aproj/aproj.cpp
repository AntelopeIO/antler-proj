/// @copyright See `LICENSE` in the root directory of this project.

#include <iostream>
#include <string>
#include <string_view>
#include <filesystem>
#include <vector>
#include <algorithm> // std::sort

#include <boost/algorithm/string.hpp> // boost::split()

#include <antler/project/project.hpp>
#include <antler/system/exec.hpp>
#include <whereami/whereami.hpp>


#include <aproj-prefix.hpp>


namespace { // anonymous

std::string exe_name;

/// Structure to store contents of subcommands/flags
struct app_entry {
   std::vector<std::string> args; ///< flags to slect this subcomand (e.g. {"-q", "--quiet"}).
   std::filesystem::path path;    ///< path to the executable subcommand.
   std::string arg_str;           ///< String representation of args (e.g. "-q, --quiet").
   std::string brief;             ///< Brief description adequate for displaying what the subcommand does.
};
std::vector<app_entry> apps;


/// Print usage information to std::cout and return 0 or, optionally - if err is not-empty() - print to std::cerr and return -1.
/// @param err  An error string to print. If empty, print to std::cout and return 0; otherwise std::cerr and return -1.
/// @return 0 if err.empty(); otherwise -1. This value is suitable for using at exit.
int usage(std::string_view err = "") {

   constexpr std::string_view help_arg{ "--help" };

   // Determine how wide the flags "clumn" should be.
   size_t width = help_arg.size();
   for (const auto& a : apps) {
      width = std::max(width, a.arg_str.size());
   }
   width += 3;

   // Sort the subcommands based on flag value.
   std::sort(apps.begin(), apps.end(), [](const app_entry& l, const app_entry& r) { return l.arg_str < r.arg_str; });

   // Determine target ostream.
   std::ostream& os = (err.empty() ? std::cout : std::cerr);

   os << exe_name << ": COMMAND [options]\n"
      << "\n"
      << " Commands:\n";

   // Print the flags followed by brief for each subcommand.
   for (const auto& a : apps) {
      std::string pad(width - a.arg_str.size(), ' ');
      os << "  " << a.arg_str << pad << a.brief << '\n';
   }

   // add help:
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
/// @param exe  The path for the subcommand.
/// @param begin  Iterator to the beginning of a list of arguments that will be passed.
/// @param end  Iterator to the end of a list of arguments that will be passed.
/// @param cmd  The flag/command used to indicate which exe to call.
/// @return The result of calling exe with the given arguments.
template<typename iterator_type>
int exec_helper(std::filesystem::path exe, iterator_type begin, iterator_type end, std::string_view cmd) {

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

}


int main(int argc, char** argv) {

   std::filesystem::path bin_path = eosio::cdt::whereami::where();
   std::filesystem::path project_path = std::filesystem::current_path();

   // Update globals - these are for the usage() function and in the arg list decoder.
   exe_name = std::filesystem::path(argv[0]).filename().string();
   // Get the sub commands.
   for (auto const& entry : std::filesystem::directory_iterator{ bin_path }) {
      const auto path = entry.path();
      if (!path.stem().string().starts_with(project_prefix))
         continue;
      // Get the brief description from the subcommand:
      auto result = antler::system::exec(path.string() + " --brief");
      if (!result) {
         std::cerr << "failed for " << path << '\n';
      } else {
         auto spc = result.output.find_first_of(' ');
         if (spc != std::string::npos) {
            // Build an app entry from the path and results of capturing the brief output.
            app_entry ae;
            ae.path = path;
            ae.arg_str = result.output.substr(0, spc);
            boost::split(ae.args, ae.arg_str, boost::is_any_of(","));
            ae.brief = result.output.substr(spc + 1);
            while (ae.brief.back() == '\n')
               ae.brief.pop_back();
            apps.push_back(ae);
         }
      }
   }

   if (argc < 2)
      return usage();

   std::vector<std::string_view> args;
   for (int i = 1; i < argc; ++i)
      args.push_back(argv[i]);

   for (auto i = args.begin(); i != args.end(); ++i) {

      if (*i == "help" || *i == "--help")
         return usage();

      // look for "--<subcommand>"
      for (const auto& a : apps) {
         for (const auto& test_arg : a.args) {
            if (*i == test_arg || (std::string{ "--" } + std::string(*i) == test_arg))
               return exec_helper(a.path, ++i, args.end(), *i);
         }
      }

      // Behave more like git, apt, ninja and other tools for 'add' command.
      if (*i == "add") {
         if (++i == args.end())
            return usage("`add` requires sub command (e.g. `add lib`).");
         std::string cmd{ "--add-" };
         cmd += *i;

         std::string real_cmd{ "add " };
         real_cmd += *i;

         for (const auto& a : apps) {
            for (const auto& test_arg : a.args) {
               if (cmd == test_arg)
                  return exec_helper(a.path, ++i, args.end(), real_cmd);
            }
         }
      }
      // return system_helper(bin_path, aproj_cmd::validate, project_path.string(), ++i, args.end(), indirect(*i));

      return usage(std::string("Bad argument: ") + std::string(*i));
   }

   return usage("No command supplied.");
}
