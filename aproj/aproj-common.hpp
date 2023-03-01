#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <cctype>               // std::isxdigit()
#include <cstdlib>              // std::exit()

#include <CLI11.hpp>

#include <antler/project/dependency.hpp>
#include <antler/project/object.hpp>

#include <aproj-prefix.hpp>


/// Load project.
/// @param path  Path to the project root or project file.
/// @return The loaded project.
inline std::optional<antler::project::project> load_project(std::filesystem::path& path) noexcept {
}


/// Load project or exit function.
/// @param cli  The CLI app to call exit from if things go poorly.
/// @param path  Path to the project root or project file.
/// @return The loaded project.
inline antler::project::project load_project_or_exit(CLI::App& cli, std::filesystem::path& path) noexcept {

   // Get the path to the project.
   if (!antler::project::project::update_path(path))
      std::exit( cli.exit( CLI::Error("path","path either did not exist or no `project.yaml` file could be found.") ) );

   // Load the project.
   auto optional_proj = antler::project::project::parse(path);
   if (!optional_proj)
      std::exit( cli.exit( CLI::Error("path", "Failed to load project file.") ) );
   return optional_proj.value();
}


/// Common init function for subcommands.
/// This function sets up expected values and prints the brief string if it was command to.
/// @param argv0  argv[0] for this executable.
/// @param brief_in v alue to store as brief string.
/// @throw std::runtime_error is thrown if cli already has `--indirect` or `--brief` flags.
inline void common_init(CLI::App& cli, std::string_view argv0, const std::string& brief_in) {

   // set brief string
   std::string brief_str = brief_in;
   // set exe name for usage
   std::string exe_name = std::filesystem::path(argv0).filename().string();

   cli.description(brief_in);
   cli.name(exe_name);

   // Add hidden flags.

   // Indirect option
   auto indirect_option = cli.add_option_function<std::string>("--indirect", [&cli](const std::string& s) { cli.name(s); } );
   indirect_option->group(""); // Hide from help.

   // Brief flag. This flag dumps brief output for `aproj` to consume for it's help.
   auto brief_flag = cli.add_flag_callback("--brief", [exe_name,brief_str](){
      // Test to see if exe can be convert to a subcommand, report an error if not.
      if (!exe_name.starts_with(project_prefix)) {
         std::cerr << "Can't provide --brief for" << exe_name << '\n';
         std::exit( -1 );
      }
      // Convert exe to subcommand.
      auto exe_as_subcmd = exe_name;
      exe_as_subcmd.erase(0, project_prefix.size());
      std::cout << "--" << exe_as_subcmd << ' ' << brief_str << '\n';
      std::exit(0);
   });
   brief_flag->group(""); // Hide from help.

}


/// Print object dependencies to a stream.
/// @tparam T  The list type. Nominally this is std::vector<antler::project::object>, but could be boost::container::devector<>.
/// @param obj_list  The list of objects to interate over.
/// @param app  Set to true to print dependencies from objects of type app, otherwise they are skipped.
/// @param lib  Set to true to print dependencies from objects of type lib, otherwise they are skipped.
/// @param tst  Set to true to print dependencies from objects of type tst, otherwise they are skipped.
template<typename T>
inline void dump_obj_deps(const T& obj_list, bool app, bool lib, bool tst, std::ostream& os = std::cout) {

   // Print the types we are reporting.
   os << "Displaying dependencies from entries of type:";
   if (app)
      os << " app";
   if (lib)
      os << " lib";
   if (tst)
      os << " test";
   os << "\n";

   // Then iterate over the elements
   for (const auto& a : obj_list) {
      // Get each object's dependencies.
      const auto& list = a.dependencies();
      // Go to the next object if either the list is empty or the object type is not one intended for printing.
      if (list.empty())
         continue;
      switch (a.type()) {
         case antler::project::object::type_t::app:
            if (!app)
               continue;
            break;
         case antler::project::object::type_t::lib:
            if (!lib)
               continue;
            break;
         case antler::project::object::type_t::test:
            if (!tst)
               continue;
            break;
         case antler::project::object::type_t::none:
         case antler::project::object::type_t::any:
            std::cerr << "Unexpected type: " << a.type() << " in object: " << a.name() << "\n";
            continue;
      }
      // Print the deps.
      auto i = list.begin();
      os << "  " << a.name() << " [" << a.type() << "]: " << i->name();
      for (++i; i != list.end(); ++i)
         os << ", " << i->name();
      os << "\n";
   }
}


/// Print object dependencies to a stream.
/// @tparam T  The list type. Nominally this is std::vector<antler::project::object>, but could be boost::container::devector<>.
/// @param obj_list  The list of objects to interate over.
template<typename T>
inline void dump_obj_deps(const T& obj_list, std::ostream& os = std::cout) {
   dump_obj_deps(obj_list, true, true, true, os);
}


/// Ask the user "Is this is correct?" and report the result.
/// @param msg  The message to print.
/// @return  The result of the query: true indicates yes, correct; false indicates no, incorrect.
[[nodiscard]] inline bool is_this_correct(std::string_view msg = "Is this correct?") noexcept {
   std::string yn = "x"; // yes or no?
   while (yn != "y" && yn != "n") {
      std::cout << msg << " [Y/n]" << std::flush;
      std::getline(std::cin, yn);
      for (auto& a : yn)
         a = static_cast<std::string::value_type>(tolower(a));
      if (yn.empty())
         yn = "y";
   }
   return yn == "y";
}


/// Test to see if an object (app/lib/test) name is valid.
/// @param s  An object name.
/// @return true indicates s was valid; false otherwise.
[[nodiscard]] inline bool validate_name(std::string_view s) noexcept {
   if (s.empty())
      return false;
   for (auto a : s) {
      if (!isalnum(a) && a != '_') // && a != '-'
         return false;
   }
   return true;
}


/// Test to see if a hash is valid.
/// @param s  The hash string to check.
/// @param byte_count  Expected size of the hash in bytes.
/// @return true if s is `byte_count` bytes long and contains only valid hex values.
[[nodiscard]] inline bool validate_hash(std::string_view s, size_t byte_count = 32) noexcept {
   if (s.size() != byte_count)
      return false;
   for (auto a : s) {
      // Return false if any digit isn't a valid hex value.
      if (!std::isxdigit(a)) {
         return false;
      }
   }
   return true;
}



/// Ask the user for a string value. Use validator to validate the result.
/// @param label  The driendly name/label/text to display in the prompt.
/// @param str  Reference to the value to set. Comes in as default value.
/// @param validator  A function to validate str with.
/// @param allow_empty  Set to true to allow the user to clear the value.
template <typename F>
inline void get_valid_string(std::string_view friendly_name, std::string& str, F&& validator, bool allow_empty = false) noexcept {
   // Loop until return.
   for (;;) {
      // Start with the text to display to the user.
      std::cout << "Enter " << friendly_name;
      if (allow_empty)
         std::cout << " (space to clear)";
      std::cout << ": [" << str << "]" << std::flush;

      // Get the input.
      std::string temp;
      std::getline(std::cin, temp);
      // Test to see if the input was empty.
      if (temp.empty()) {
         // If it was, the default is taken if it exists, or empty if that's a valid option.
         if (!str.empty() || allow_empty)
            return;
      }
      else {
         // We use a single space to clear the string...
         if (temp == " ") {
            // ...but only if allow empty is set.
            if (allow_empty) {
               str.clear();
               return;
            }
         }
         // Otherwise, test to see if the string is valid.
         else if (!temp.empty() && validator(temp)) {
            str = temp;
            return;
         }
      }
   }
}


/// Ask the user for a string value. Do not validate result.
/// @param friendly_name  The label/text to display in the prompt.
/// @param str  Reference to the value to set. Comes in as default value.
/// @param allow_empty  Set to true to allow the user to clear the value.
inline void get_string(std::string_view friendly_name, std::string& str, bool allow_empty = false) noexcept {
   auto always_valid = [](std::string_view)->bool { return true; };
   return get_valid_string(friendly_name, str, always_valid, allow_empty);
}


/// Ask the user for a value. Validated with validate_name().
/// @param friendly_name  The label/text to display in the prompt.
/// @param name  Reference to the value to set. Comes in as default value.
/// @param allow_empty  Set to true to allow the user to clear the value.
inline void get_name(std::string_view friendly_name, std::string& name, bool allow_empty = false) noexcept {
   return get_valid_string(friendly_name, name, validate_name, allow_empty);
}



/// Ask the user for a hash. Validated with validate_hash().
/// @param friendly_name  The label/text to display in the prompt.
/// @param hash  Reference to the value to set. Comes in as default value.
/// @param allow_empty  Set to true to allow the user to clear the value.
inline void get_hash(std::string_view friendly_name, std::string& hash, bool allow_empty = false) noexcept {
   auto validator = [](std::string_view s) -> bool { return validate_hash(s,32); };
   return get_valid_string(friendly_name, hash, validator, allow_empty);
}


/// Ask the user for a location value. Validated with antler::project::dependency::validate_location().
/// @param friendly_name  The label/text to display in the prompt.
/// @param loc  Reference to the value to set. Comes in as default value.
/// @param allow_empty  Set to true to allow the user to clear the value.
inline void get_loc(std::string_view friendly_name, std::string& loc, bool allow_empty = false) noexcept {
   auto validator = [](std::string_view s) -> bool { return antler::project::dependency::validate_location(s); };
   return get_valid_string(friendly_name, loc, validator, allow_empty);
}

/// Ask the user for a language value.
/// @param friendly_name  The label/text to display in the prompt.
/// @param lang  Reference to the language to set.
/// @param allow_empty  Set to true to allow the user to "clear" the value (i.e. set the value to language::none).
inline void get_language(std::string_view friendly_name, antler::project::language& lang, bool allow_empty = false) noexcept {
   auto validator = [allow_empty](std::string_view s) -> bool {
      if(allow_empty && s == "none")
         return true;
      antler::project::language l = antler::project::to_language(s);
      return l != antler::project::language::none;
   };
   std::string temp = antler::project::to_string(lang);
   get_valid_string(friendly_name, temp, validator, allow_empty);
   lang = antler::project::to_language(temp);
}


/// Ask the user for a version value.
/// @param friendly_name  The label/text to display in the prompt.
/// @param ver  Reference to the version to set.
/// @param allow_empty  Set to true to allow the user to clear the value.
inline void get_version(std::string_view friendly_name, antler::project::version& ver, bool allow_empty = false) noexcept {
   std::string s = std::string{ver.raw()};
   get_string(friendly_name,s,allow_empty);
   ver = s;
}
