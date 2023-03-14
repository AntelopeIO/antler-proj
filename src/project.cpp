/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.hpp>

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

namespace detail {
   template <typename List>
   inline static typename std::remove_reference_t<List>::value_type* find_if(List&& objs, std::string_view name) {
      auto tmp = std::find_if(objs.begin(), objs.end(), [name](const auto& o) { return o.name() == name; });
      if (tmp != objs.end()) {
         return &*tmp;
      } else {
         return nullptr;
      }
   }
   
}

antler::project::object& project::object(std::string_view name) {
   if (auto ptr = detail::find_if(m_apps, name); ptr != nullptr)
      return *ptr;
   if (auto ptr = detail::find_if(m_libs, name); ptr != nullptr)
      return *ptr;
   if (auto ptr = detail::find_if(m_tests, name); ptr != nullptr)
      return *ptr;

   throw std::runtime_error("object not found.");
}

bool project::object_exists(std::string_view name, object::type_t type) const noexcept {
   const auto& exists = [&](auto objs) { return detail::find_if(objs, name) != nullptr; };

   switch (type) {
      case object::type_t::app:
         return exists(m_apps);
      case object::type_t::lib:
         return exists(m_libs);
      case object::type_t::test:
         return exists(m_tests);
      case object::type_t::any:
         return exists(m_apps) || exists(m_libs) || exists(m_tests);
      default:
         return false;
   }
}


std::filesystem::path project::path() const noexcept {
   return m_path;
}


void project::path(const std::filesystem::path& path) noexcept {
   m_path = path;
}


bool project::remove(std::string_view name, object::type_t type) noexcept {

   bool rv = false;

   if (type == object::type_t::any || type == object::type_t::app) {
      auto i = std::find_if(m_apps.begin(), m_apps.end(), [name](const antler::project::object& o) { return o.name() == name; });
      if (i != m_apps.end()) {
         m_apps.erase(i);
         rv = true;
      }
   }

   if (type == object::type_t::any || type == object::type_t::lib) {
      auto i = std::find_if(m_libs.begin(), m_libs.end(), [name](const antler::project::object& o) { return o.name() == name; });
      if (i != m_libs.end()) {
         m_libs.erase(i);
         rv = true;
      }
   }

   if (type == object::type_t::any || type == object::type_t::test) {
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
      out.flush();
   }
   catch(std::exception& e) {
      es << "Exception: " << e.what() << "\n";
      return false;
   }

   return true;
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

bool project::validate_dependency(const dependency& dep, std::ostream& errs) const noexcept {
   if (dep.location().empty()) {
      return object_exists(dep.name(), object::type_t::lib);
   } else if (!dep.is_valid_location()) {
      errs << "Error dependency: " << dep.name() << " is invalid." << std::endl;
      return false;
   }
   return true;
}

bool project::has_valid_dependencies(std::ostream& errs) const noexcept {
   const auto& test_deps = [&](auto objs) {
      for (const auto& o : objs) {
         for (const auto& d : o.dependencies()) {
            if (!validate_dependency(d, errs))
               return false;
         }
      }
      return true;
   };

   return test_deps(m_apps) && test_deps(m_libs) && test_deps(m_tests);
}


} // namespace antler::project
