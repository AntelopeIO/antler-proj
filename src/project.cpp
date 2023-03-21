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
bool project::init_dirs(const system::fs::path& path, std::ostream& error_stream) noexcept {

   std::error_code sec;

   // Create the root directory.
   system::fs::create_directories(path, sec);
   if (sec) {
      error_stream << path << " could not be created: " << sec << "\n";
      return false;
   }

   // Create the directory structure.
   {
      const std::vector<system::fs::path> files = { "apps", "include", "ricardian", "libs", "tests" };
      for (const auto& fn : files) {
         system::fs::create_directory(path/fn, sec);
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
      yaml::write(m_path / project::manifest_name, to_yaml());
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
      if (system::fs::exists(search_path / "project.yaml")) {
         path = search_path / "project.yaml";
         return true;
      }
      if (system::fs::exists(search_path / "project.yml")) {
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

bool project::populate_dependency(const dependency& d, uint32_t jobs) {
   using namespace antler::project::location;
   system::fs::path depends_dir = m_path / dependencies_dir;
   system::debug_log("populating dependency {0} from {1}", d.name(), d.location());
   std::string tag = d.tag();
   
   if (m_deps.find(d.location()) != m_deps.end())
      return true;

   m_deps.emplace(d.location());
   if (is_reachable(d.location())) {
      if (is_github_repo(d.location())) {
         if (d.tag().empty()) {
            tag = "main";
         }
         return clone_git_repo(d.location(), tag, jobs, depends_dir / d.name());
      } else if (is_github_shorthand(d.location())) {
         std::string org = std::string{github::get_org(d.location())};
         std::string repo = std::string{github::get_repo(d.location())};

         if (d.tag().empty()) {
            tag = get_github_default_branch(org, repo);
         }
         system::debug_log("Cloning {0} with branch {1}", d.location(), tag);
         return clone_github_repo(org, repo, tag, jobs, depends_dir / d.name());
      }
   }
   return false;
}

bool project::populate_dependencies(uint32_t jobs) {
   system::fs::path depends_dir = m_path / dependencies_dir;

   system::fs::create_directories(depends_dir);

   // set the main project as a dependency
   m_deps.emplace(m_name);

   ANTLER_CHECK(system::fs::exists(depends_dir), "internal failure, dependencies directory was not created {0}", depends_dir.string());

   const auto& pop_deps = [&](auto& objs) {
      for (const auto& [_, o] : objs) {
         for (const auto& [_, d] : o.dependencies()) {
            if (!populate_dependency(d, jobs))
               return false;
            project proj(depends_dir / d.name());
            system::debug_log("Dependency loaded {0}", proj.name());
            if (!proj.populate_dependencies(jobs)) {
               return false;
            }
         }
      }
      return true;
   };

   if (!pop_deps(m_apps))
      return false;
   return pop_deps(m_libs);
}

} // namespace antler::project
