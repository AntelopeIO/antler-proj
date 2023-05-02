/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.hpp>
#include <antler/system/version.hpp>
#include <antler/project/net_utils.hpp>
#include <antler/project/location.hpp>

#include <iostream>
#include <fstream>
#include <algorithm>            // find_if()

namespace antler::project {

//--- alphabetic --------------------------------------------------------------------------------------------------------
bool project::init_dirs(const system::fs::path& path) noexcept {

   std::error_code sec;

   // Create the root directory.
   system::fs::create_directories(path, sec);
   if (sec) {
      system::error_log("Project directories could not be created: {0} at path {1}", sec.message(), path.string());
      return false;
   }

   // Create the directory structure.
   {
      const std::vector<system::fs::path> files = { "apps", "include", "ricardian", "libs", "tests" };
      for (const auto& fn : files) {
         system::fs::create_directory(path/fn, sec);
         if (sec) {
            system::error_log("{0} could not be created: {1}", (path/fn).string(), sec.message());
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

system::fs::path project::path() const noexcept {
   return m_path;
}


void project::path(const system::fs::path& path) noexcept {
   m_path = path;
}


bool project::sync() {

   if (m_path.empty()) {
      system::error_log("Path: {0} is a valid path to write to.", m_path.string());
      return false;
   }

   try {
      auto manifest = choose_manifest(m_path);
      yaml::write(m_path / manifest, to_yaml());
      system::info_log("Wrote project manifest to {0}.", m_path.string());
   }
   catch(std::exception& e) {
      system::error_log("Exception during syncing : {0}", e.what());
      return false;
   }

   return true;
}


bool project::update_path(system::fs::path& path) noexcept {

   system::fs::path search_path = path;
   if (search_path.empty())
      search_path = system::fs::current_path();
   else if (search_path.filename().extension() == ".yaml" || search_path.filename().extension() == ".yml") {
      // The user passed in an *.yaml file, we just report if it exists as a regular file.
      return system::fs::is_regular_file(search_path);
   }

   for (;;) {
      if (system::fs::exists(search_path / manifest_name)) {
         path = search_path / manifest_name;
         return true;
      }
      // alternative manifest name
      if (system::fs::exists(search_path / manifest_alternative)) {
         path = search_path / manifest_alternative;
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

bool project::validate_dependency(const dependency& dep) const noexcept {
   if (dep.location().empty()) {
      return lib_exists(dep.name());
   } else if (!dep.is_valid_location()) {
      system::error_log("Error dependency: {0} is invalid.", dep.name());
      return false;
   }
   return true;
}

bool project::has_valid_dependencies() const noexcept {
   const auto& test_deps = [&](auto objs) {
      for (const auto& [_, o] : objs) {
         for (const auto& [_, d] : o.dependencies()) {
            if (!validate_dependency(d))
               return false;
         }
      }
      return true;
   };

   return test_deps(m_apps) && test_deps(m_libs); // && test_deps(m_tests);
}

} // namespace antler::project
