#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <cctype>               // std::isxdigit()
#include <cstdlib>              // std::exit()

#include <antler/project/dependency.hpp>
#include <antler/project/object.hpp>

#include <aproj-prefix.hpp>


// Global declarations.
std::string exe_name;
std::string brief_str;

/// Convert an exe_name into a subcommand and print. Example aproj-init <brief> become "--init <brief>".
/// @param exe_name  The executable name to convert to subcommand.
/// @param brief  The brief description of the subcommand.
inline void print_brief(std::string& exe_name_in, std::string_view brief_text) {
   exe_name_in.erase(0, project_prefix.size());
   std::cout << "--" << exe_name_in << ' ' << brief_text << '\n';
}


/// Common init function for subcommands.
/// This function sets up expected values and prints the brief string if it was command to.
/// @param argc argc
/// @param argv argv
/// @param brief_in value to store as brief string.
inline void common_init(int& argc, char** argv, std::string_view brief_in) {
   // set brief string
   brief_str = brief_in;
   // set exe name for usage
   exe_name = std::filesystem::path(argv[0]).filename().string();
   // search for indirect string
   if (argc > 0) {
      constexpr std::string_view indirect_str{ "--indirect=" };
      if (std::string_view(argv[argc - 1]).starts_with(indirect_str)) {
         exe_name = std::string_view(argv[argc - 1]).substr(indirect_str.size());
         --argc;
      }
   }
   // search for brief flag.
   for (int i = 0; i < argc; ++i) {
      if (std::string_view(argv[i]) == "--brief") {
         if (!exe_name.starts_with(project_prefix)) {
            std::cerr << "Can't provide --brief for" << argv[0] << '\n';
            std::exit( -1 );
         }
         print_brief(exe_name, brief_str);
         std::exit(0);
      }
   }
} // COMMON_INIT


/// Print object dependencies to a stream.
/// @param obj_list  The list of objects to interate over.
/// @param app  Set to true to print dependencies from objects of type app, otherwise they are skipped.
/// @param lib  Set to true to print dependencies from objects of type lib, otherwise they are skipped.
/// @param tst  Set to true to print dependencies from objects of type tst, otherwise they are skipped.
template<typename T>
inline void dump_obj_deps(const T& obj_list, bool app, bool lib, bool tst, std::ostream& os = std::cout) {
   os << "Displaying dependencies from entries of type:";
   if (app)
      os << " app";
   if (lib)
      os << " lib";
   if (tst)
      os << " test";
   os << "\n";

   for (const auto& a : obj_list) {
      const auto& list = a.dependencies();
      if (list.empty())
         continue;
      switch (a.type()) {
         case antler::project::object::type_t::app:
            if (!app) {
               continue;
            }
            break;
         case antler::project::object::type_t::lib:
            if (!lib) {
               continue;
            }
            break;
         case antler::project::object::type_t::test:
            if (!tst) {
               continue;
            }
            break;
         case antler::project::object::type_t::none:
         case antler::project::object::type_t::any:
            std::cerr << "Unexpected type: " << a.type() << " in object: " << a.name() << "\n";
            continue;
      }

      auto i = list.begin();
      os << "  " << a.name() << " [" << a.type() << "]: " << i->name();
      for (++i; i != list.end(); ++i)
         os << ", " << i->name();
      os << "\n";
   }
}


/// Print object dependencies to a stream.
/// @param obj_list  The list of objects to interate over.
template<typename T>
inline void dump_obj_deps(const T& obj_list, std::ostream& os = std::cout) {
   dump_obj_deps(obj_list, true, true, true, os);
}


/// Ask the user if this is correct.
/// @param msg  The message to print.
/// @return  The result of the query: true indicates yes, correct; false indicates no, incorrect.
inline bool is_this_correct(std::string_view msg = "Is this correct?") noexcept {
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
inline bool is_valid_name(std::string_view s) noexcept {
   if (s.empty())
      return false;
   for (auto a : s) {
      if (!isalnum(a) && a != '_') // && a != '-'
         return false;
   }
   return true;
}


/// Ask the user for a value. Validated with is_valid_name().
/// @param friendly_name  The label/text to display in the prompt.
/// @param name  Reference to the value to set. Comes in as default value.
/// @param allow_empty  Set to true to allow the user to clear the value.
inline void get_name(std::string_view friendly_name, std::string& name, bool allow_empty = false) noexcept {
   // Loop until return.
   for (;;) {
      std::cout << "Enter " << friendly_name;
      if (allow_empty)
         std::cout << " (space to clear)";
      std::cout << ": [" << name << "]" << std::flush;

      std::string temp;
      std::getline(std::cin, temp);
      // No change?
      if (temp.empty()) {
         if (allow_empty || !name.empty())
            return;
      }
      else {
         if (allow_empty && temp == " ") {
            name.clear();
            return;
         }
         if (is_valid_name(temp)) {
            name = temp;
            return;
         }
      }
   }
}


/// Test to see if a hash is valid.
/// @param s  The hash string to check.
/// @param byte_count  Expected size of the hash in bytes.
/// @return true if s is `byte_count` bytes long and contains only valid hex values.
[[nodiscard]] inline bool is_valid_hash(std::string_view s, size_t byte_count = 32) noexcept {
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


/// Ask the user for a hash. Validated with is_valid_hash().
/// @param friendly_name  The label/text to display in the prompt.
/// @param hash  Reference to the value to set. Comes in as default value.
/// @param allow_empty  Set to true to allow the user to clear the value.
inline void get_hash(std::string_view friendly_name, std::string& hash, bool allow_empty = false) noexcept {
   // Loop until return.
   for (;;) {
      std::cout << "Enter " << friendly_name;
      if (allow_empty)
         std::cout << " (space to clear)";
      std::cout << ": [" << hash << "]" << std::flush;

      std::string temp;
      std::getline(std::cin, temp);
      // No change?
      if (temp.empty()) {
         if (allow_empty || !hash.empty())
            return;
      }
      else {
         if (allow_empty && temp == " ") {
            hash.clear();
            return;
         }
         if (is_valid_hash(temp)) {
            hash = temp;
            return;
         }
      }
   }
}


/// Ask the user for a location value. Validated with antler::project::dependency::validate_location().
/// @param friendly_name  The label/text to display in the prompt.
/// @param loc  Reference to the value to set. Comes in as default value.
/// @param allow_empty  Set to true to allow the user to clear the value.
inline void get_loc(std::string_view friendly_name, std::string& loc, bool allow_empty = false) noexcept {
   // Loop until return.
   for (;;) {
      std::cout << "Enter " << friendly_name;
      if (allow_empty)
         std::cout << " (space to clear)";
      std::cout << ": [" << loc << "]" << std::flush;

      std::string temp;
      std::getline(std::cin, temp);
      // No change?
      if (temp.empty()) {
         if (allow_empty || !loc.empty())
            return;
      }
      else {
         if (allow_empty && temp == " ") {
            loc.clear();
            return;
         }
         if (antler::project::dependency::validate_location(temp)) {
            loc = temp;
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
   // Loop until return.
   for (;;) {
      std::cout << "Enter " << friendly_name;
      if (allow_empty)
         std::cout << " (space to clear)";
      std::cout << ": [" << str << "]" << std::flush;

      std::string temp;
      std::getline(std::cin, temp);
      // No change?
      if (temp.empty()) {
         if (allow_empty || !str.empty())
            return;
      }
      else {
         if (temp == " ") {
            if (allow_empty) {
               str.clear();
               return;
            }
         }
         else if (!temp.empty()) {
            str = temp;
            return;
         }
      }
   }
}
