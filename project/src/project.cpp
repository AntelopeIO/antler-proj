/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.h>

#include <iostream>
#include <fstream>
#include <algorithm>            // find_if()
#include <key.h>

namespace antler {
namespace project {


//--- constructors/destrructor ------------------------------------------------------------------------------------------

project::project() = default;
/*
project::project(const char* filename) {
   parse(filename);
}


project::project(const std::filesystem::path& filename) {
   parse(filename);
}
*/

//--- alphabetic --------------------------------------------------------------------------------------------------------

object::list_t project::all_objects() const noexcept {
   auto rv = m_apps;
   rv.reserve(m_apps.size() + m_libs.size() + m_tests.size());
   for (auto a : m_libs)
      rv.emplace_back(a);
   for (auto a : m_tests)
      rv.emplace_back(a);
   return rv;
}


const object::list_t& project::apps() const noexcept {
   return m_apps;
}


bool project::init_dirs(const std::filesystem::path& path, bool expect_empty, std::ostream& error_stream) noexcept {

   std::error_code sec;

   // Create the root directory.
   std::filesystem::create_directories(path, sec);
   if (sec) {
      error_stream << path << " could not be created: " << sec << "\n";
      return false;
   }

   if (expect_empty && !std::filesystem::is_empty(path, sec)) {
      error_stream << path << " is NOT empty!\n";
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


const object::list_t& project::libs() const noexcept {
   return m_libs;
}


std::string_view project::name() const noexcept {
   return m_name;
}


void project::name(std::string_view s) noexcept {
   m_name = s;
}


std::optional<antler::project::object> project::object(std::string_view name) const noexcept {

   auto rv = std::find_if(m_apps.begin(), m_apps.end(), [name](const auto& o) { return o.name() == name; });
   if (rv != m_apps.end())
      return *rv;

   rv = std::find_if(m_libs.begin(), m_libs.end(), [name](const auto& o) { return o.name() == name; });
   if (rv != m_libs.end())
      return *rv;

   rv = std::find_if(m_tests.begin(), m_tests.end(), [name](const auto& o) { return o.name() == name; });
   if (rv != m_tests.end())
      return *rv;

   return std::optional<antler::project::object>{};
}


bool project::object_exists(std::string_view name, object::type_t type) const noexcept {

   if (type == object::type_t::any || type == object::type_t::app) {
      auto i = std::find_if(m_apps.begin(), m_apps.end(), [name](const auto& o) { return o.name() == name; });
      if (i != m_apps.end())
         return true;
   }

   if (type == object::type_t::any || type == object::type_t::lib) {
      auto i = std::find_if(m_libs.begin(), m_libs.end(), [name](const auto& o) { return o.name() == name; });
      if (i != m_libs.end())
         return true;
   }

   if (type == object::type_t::any || type == object::type_t::test) {
      auto i = std::find_if(m_tests.begin(), m_tests.end(), [name](const auto& o) { return o.name() == name; });
      if (i != m_tests.end())
         return true;
   }

   return false;
}


std::filesystem::path project::path() const noexcept {
   return m_path;
}


void project::path(const std::filesystem::path& path) noexcept {
   m_path = path;
}


bool project::remove(std::string_view name, object::type_t type) noexcept {

   bool rv = false;

   if (type == object::any || type == object::app) {
      auto i = std::find_if(m_apps.begin(), m_apps.end(), [name](const antler::project::object& o) { return o.name() == name; });
      if (i != m_apps.end()) {
         m_apps.erase(i);
         rv = true;
      }
   }

   if (type == object::any || type == object::lib) {
      auto i = std::find_if(m_libs.begin(), m_libs.end(), [name](const antler::project::object& o) { return o.name() == name; });
      if (i != m_libs.end()) {
         m_libs.erase(i);
         rv = true;
      }
   }

   if (type == object::any || type == object::test) {
      auto i = std::find_if(m_tests.begin(), m_tests.end(), [name](const antler::project::object& o) { return o.name() == name; });
      if (i != m_tests.end()) {
         m_tests.erase(i);
         rv = true;
      }
   }

   return rv;
}


bool project::sync(std::ostream& es) noexcept {

   if (m_path.empty()) {
      es << "No path to write to.\n";
      return false;
   }


   try {

      // Open the file.
      std::ofstream out(m_path);
      if (!out.is_open()) {
         es << "Problem opening " << m_path << "\n";
         return false;
      }
      // Print this project to the file.
      print(out);
   }
   catch(std::exception& e) {
      es << "Exception: " << e.what() << "\n";
      return false;
   }

   // Now, truly sync.
   return system("sync") == 0;
}


const object::list_t& project::tests() const noexcept {
   return m_tests;
}


std::string project::to_yaml() const noexcept {

   // ryml wants to print to a stream, so use a std::stringstream here.
   std::stringstream ss;
   print(ss);
   return ss.str();
}


bool project::update_path(std::filesystem::path& path) noexcept {

   std::error_code sec;

   std::filesystem::path search_path = path;
   if (search_path.empty())
      search_path = std::filesystem::current_path();
   else if (search_path.filename().extension() == ".yaml" || search_path.filename().extension() == ".yml") {
      // The user passed in an *.yaml file, we just report if it exists as a regular file.
      return std::filesystem::is_regular_file(search_path, sec);
   }

   for (;;) {
      if (std::filesystem::exists(search_path / "project.yaml", sec)) {
         path = search_path / "project.yaml";
         return true;
      }
      if (std::filesystem::exists(search_path / "project.yml", sec)) {
         path = search_path / "project.yml";
         return true;
      }
      if (search_path.empty() || search_path == "/")
         break;
      search_path = search_path.parent_path();
   }
   return false;
}


void project::upsert(antler::project::object&& obj) noexcept {

   switch (obj.type()) {
      case object::app: upsert_app(std::move(obj)); break;
      case object::lib: upsert_lib(std::move(obj)); break;
      case object::test: upsert_test(std::move(obj)); break;

      case object::any:
      case object::none:
         // error state!
         std::cerr << "Failed to upsert object with name and type: " << obj.name() << ", " << obj.type() << std::endl;
         return;

         // Never add a default.
   }
}


void project::upsert_app(antler::project::object&& app) noexcept {

   auto i = std::find_if(m_apps.begin(), m_apps.end(), [app](const antler::project::object& o) { return o.name() == app.name(); });
   if (i != m_apps.end())
      *i = app;
   else
      m_apps.emplace_back(app);
}


void project::upsert_lib(antler::project::object&& lib) noexcept {

   auto i = std::find_if(m_libs.begin(), m_libs.end(), [lib](const antler::project::object& o) { return o.name() == lib.name(); });
   if (i != m_libs.end())
      *i = lib;
   else
      m_libs.emplace_back(lib);
}


void project::upsert_test(antler::project::object&& test) noexcept {

   auto i = std::find_if(m_tests.begin(), m_tests.end(), [test](const antler::project::object& o) { return o.name() == test.name(); });
   if (i != m_tests.end())
      *i = test;
   else
      m_tests.emplace_back(test);
}


antler::project::version project::version() const noexcept {
   return m_ver;
}


void project::version(const antler::project::version& ver) noexcept {
   m_ver = ver;
}


} // namespace project
} // namespace antler
