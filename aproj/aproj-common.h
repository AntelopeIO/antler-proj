#ifndef antler_aproj_aproj_common_h
#define antler_aproj_aproj_common_h

#include <stdlib.h>
#include <iostream>
#include <filesystem>

#include <antler/project/dependency.h>
#include <antler/project/object.h>

const std::string_view project_prefix{ "aproj-" };

inline void print_brief(std::string& exe_name, const std::string& brief_text) {
   exe_name.erase(0, project_prefix.size());
   // constexpr size_t width{17};
   // const size_t space_count = (exe_name.size() < width ? width-exe_name.size() : 2);
   // exe_name.append(space_count,' ');
   std::cout << "--" << exe_name << ' ' << brief_text << '\n';
}


#define RETURN_USAGE(X)       \
   {                          \
      std::stringstream ss;   \
      ss X;                   \
      return usage(ss.str()); \
   }

#define COMMON_INIT(BRIEF_TEXT)                                                      \
   {                                                                                 \
      /* set exe name for usage */                                                   \
      exe_name = std::filesystem::path(argv[0]).filename().string();                 \
      /* search for indirect string */                                               \
      if (argc > 0) {                                                                \
         constexpr std::string_view indirect_str{ "--indirect=" };                   \
         if (std::string_view(argv[argc - 1]).starts_with(indirect_str)) {           \
            indirect = std::string_view(argv[argc - 1]).substr(indirect_str.size()); \
            exe_name = indirect;                                                     \
            --argc;                                                                  \
         }                                                                           \
      }                                                                              \
      /* search for brief and help flags */                                          \
      for (int i = 0; i < argc; ++i) {                                               \
         if (std::string_view(argv[i]) == "--help")                                  \
            return usage("");                                                        \
         if (std::string_view(argv[i]) == "--brief") {                               \
            if (!exe_name.starts_with(project_prefix)) {                             \
               std::cerr << "Can't provide --brief for" << argv[0] << '\n';          \
               return -1;                                                            \
            }                                                                        \
            print_brief(exe_name, BRIEF_TEXT);                                       \
            return 0;                                                                \
         }                                                                           \
      }                                                                              \
   } // COMMON_INIT



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


template<typename T>
inline void dump_obj_deps(const T& obj_list, std::ostream& os = std::cout) {
   dump_obj_deps(obj_list, true, true, true, os);
}


/// ask the user if this is correct.
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
inline bool is_valid_name(std::string_view s) noexcept {
   if (s.empty())
      return false;
   for (auto a : s) {
      if (!isalnum(a) && a != '_') // && a != '-'
         return false;
   }
   return true;
}



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
      } else {
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
inline bool is_valid_hash(std::string_view s, size_t byte_count = 32) noexcept {
   if (s.size() != byte_count)
      return false;
   for (auto a : s) {
      if (!(a >= '0' && a <= '9') && !(a >= 'a' && a <= 'f') && !(a >= 'A' && a <= 'F')) {
         return false;
      }
   }
   return true;
}


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
      } else {
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
      } else {
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



#endif
