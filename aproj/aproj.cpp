#include <iostream>
#include <string>
#include <string_view>
#include <filesystem>
#include <vector>
#include <algorithm> // std::sort

#include <antler/project/project.h>
#include <antler/string/split.h>
#include <antler/system/exec.h>
#include <sb/filesystem/executable_path.h>


#include <aproj-common.h>


namespace { // anonymous

std::string exe_name;

struct app_entry {
   std::vector<std::string> args;
   std::filesystem::path path;
   std::string arg_str;
   std::string brief;
};
std::vector<app_entry> apps;


int usage(std::string_view err = "") {

   constexpr std::string_view help_arg{ "--help" };

   size_t width = help_arg.size();
   for (const auto& a : apps) {
      width = std::max(width, a.arg_str.size());
   }
   width += 3;

   std::sort(apps.begin(), apps.end(), [](const app_entry& l, const app_entry& r) { return l.arg_str < r.arg_str; });

   std::ostream& os = (err.empty() ? std::cout : std::cerr);

   os << exe_name << ": COMMAND [options]\n"
      << "\n"
      << " Commands:\n";

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
   if (err.empty())
      return 0;
   os << "Error: " << err << "\n";
   return -1;
}


template<typename iterator_type>
int exec_helper(std::filesystem::path exe, iterator_type begin, iterator_type end, std::string_view cmd) {

   std::stringstream ss;
   ss << exe;
   for (auto i = begin; i < end; ++i)
      ss << " " << *i;
   ss << " --indirect=\"" << exe_name << ' ' << cmd << '"';

   return system(ss.str().c_str());
}

}


int main(int argc, char** argv) {


   std::filesystem::path bin_path = sb::filesystem::executable_path().parent_path();
   std::filesystem::path project_path = std::filesystem::current_path();

   // Update globals - these are for the usage() function and in the arg list decoder.
   exe_name = std::filesystem::path(argv[0]).filename().string();
   // Get the sub commands.
   for (auto const& entry : std::filesystem::directory_iterator{ bin_path }) {
      const auto path = entry.path();
      if (!path.stem().string().starts_with(project_prefix))
         continue;
      auto result = antler::system::exec(path.string() + " --brief");
      if (!result) {
         std::cerr << "failed for " << path << '\n';
      } else {
         auto spc = result.output.find_first_of(' ');
         if (spc != std::string::npos) {
            app_entry ae;
            ae.path = path;
            ae.arg_str = result.output.substr(0, spc);
            ae.args = antler::string::split<std::string>(ae.arg_str, ",");
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

      for (const auto& a : apps) {
         for (const auto& test_arg : a.args) {
            if (*i == test_arg || (std::string{ "--" } + std::string(*i) == test_arg))
               return exec_helper(a.path, ++i, args.end(), *i);
         }
      }

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
