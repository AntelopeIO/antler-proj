/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.hpp>
#include <antler/system/version.hpp>

#include <iostream>
#include <fstream>
#include <algorithm>            // find_if()

namespace antler::project {

//--- alphabetic --------------------------------------------------------------------------------------------------------
bool project::init_dirs(const std::filesystem::path& path, std::ostream& error_stream) noexcept {

   std::error_code sec;

   // Create the root directory.
   std::filesystem::create_directories(path, sec);
   if (sec) {
      error_stream << path << " could not be created: " << sec << "\n";
      return false;
   }

   // Create the directory structure.
   {
      const std::vector<std::filesystem::path> files = { "apps", "include", "ricardian", "libs", "tests" };
      for (const auto& fn : files) {
         std::filesystem::create_directory(path/fn, sec);
         if (sec) {
            error_stream << (path/fn) << " could not be created: " << sec << "\n";
            return false;
         }
      }
   }
   return true;
}

std::string_view project::name() const noexcept {
   return m_name;
}


void project::name(std::string_view s) noexcept {
   m_name = s;
}

std::filesystem::path project::path() const noexcept {
   return m_path;
}


void project::path(const std::filesystem::path& path) noexcept {
   m_path = path;
}


bool project::sync() noexcept {

   if (m_path.empty()) {
      system::error_log("Path: {0} is a valid path to write to.", m_path.string());
      return false;
   }


   try {
      yaml::write(m_path / manifest_name, to_yaml());
   }
   catch(std::exception& e) {
      system::error_log("Exception during syncing : {0}", e.what());
      return false;
   }

   return true;
}


bool project::update_path(std::filesystem::path& path) noexcept {

   std::filesystem::path search_path = path;
   if (search_path.empty())
      search_path = std::filesystem::current_path();
   else if (search_path.filename().extension() == ".yaml" || search_path.filename().extension() == ".yml") {
      // The user passed in an *.yaml file, we just report if it exists as a regular file.
      return std::filesystem::is_regular_file(search_path);
   }

   for (;;) {
      if (std::filesystem::exists(search_path / "project.yaml")) {
         path = search_path / "project.yaml";
         return true;
      }
      if (std::filesystem::exists(search_path / "project.yml")) {
         path = search_path / "project.yml";
         return true;
      }
      if (search_path.empty() || search_path == "/")
         return false;

      search_path = search_path.parent_path();
   }
   
   return false;
}


antler::project::version project::version() const noexcept {
   return m_ver;
}


void project::version(const antler::project::version& ver) noexcept {
   m_ver = ver;
}

bool project::validate_dependency(const dependency& dep, std::ostream& errs) const noexcept {
   if (dep.location().empty()) {
      return lib_exists(dep.name());
   } else if (!dep.is_valid_location()) {
      errs << "Error dependency: " << dep.name() << " is invalid." << std::endl;
      return false;
   }
   return true;
}

bool project::has_valid_dependencies(std::ostream& errs) const noexcept {
   const auto& test_deps = [&](auto objs) {
      for (const auto& [_, o] : objs) {
         for (const auto& [_, d] : o.dependencies()) {
            if (!validate_dependency(d, errs))
               return false;
         }
      }
      return true;
   };

   return test_deps(m_apps) && test_deps(m_libs); // && test_deps(m_tests);
}

/*
void project::print(std::ostream& os) const noexcept {
   // Add a header.
   os << magic_comment << "\n";
   os << comment_preamble << "\n";
   os << "#   generated with v" << system::version::full() << std::endl;

   os << "#\n"
      << "# This file was auto-generated. Be aware antler-proj may discard added comments.\n"
      << "\n\n";
   os << to_yaml();
}
*/

} // namespace antler::project
