#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include <filesystem>
#include <optional>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>


#include "object.hpp"
#include "version.hpp"

namespace antler::project {

/// This class represents the contents of a `project.yaml` file. Functions exist to encode and decode from a `project.yaml` file.
class project {

public:
   // parse a project from a project.yml

   constexpr static inline std::string_view manifest_name = "project.yml";
   constexpr static inline std::string_view magic_comment = "#antler-proj::generated";
   constexpr static inline std::string_view comment_preamble = "#   This file was AUTOGENERATED.\n"
                                                               "#   Modification or removal of the above lines will cause antler-proj to deem the project as invalid.\n"
                                                               "#   Any changes made to this file will be lost when the tool updates this project.";
   // constructors
   project() = default;
   project(const std::filesystem::path& p) 
      : m_path(p) {
      system::debug_log("project(const std::filesystem::path&) called with path : {0}" , p.string());
      if (!from_yaml(yaml::load(p / manifest_name)))
         throw std::runtime_error("project can't be created from path"); 
   }
   project(const std::filesystem::path& path, std::string_view name, std::string_view version_raw) :
      m_path(path), m_name(name), m_ver(version_raw) {
      system::debug_log("project(const std::filesystem::path&, std::string_view, std::string_view) called with path : {0}, name : {1}, version : {2}"
         , path.string(), name, version_raw); 
   }

   /// Get the project name.
   /// @return  The name of the project.
   [[nodiscard]] std::string_view name() const noexcept;
   /// Set the project name.
   /// @param s  The new name of the project.
   void name(std::string_view s) noexcept;

   /// Get the path to the project. This is the actual `project.yaml` file, not the containing directory.
   /// @return  The path to the project file.
   [[nodiscard]] std::filesystem::path path() const noexcept;
   /// Set the path to the project. This is the actual `project.yaml` file, not the containing directory.
   /// @param path  The new path to the project file.
   void path(const std::filesystem::path& path) noexcept;

   /// Get this project's version info.
   /// @return  The version information.
   [[nodiscard]] antler::project::version version() const noexcept;
   /// Set this project's version info.
   /// @param ver  The new version information.
   void version(const antler::project::version& ver) noexcept;

   /// Remove an app from the project.
   /// @param name  The name of the app to remove.
   /// @return True if the app existed and was erased, false otherwise.
   [[nodiscard]] inline bool remove_app(const std::string& name) noexcept { return m_apps.erase(name) > 0; }

   /// Remove a lib from the project.
   /// @param name  The name of the lib to remove.
   /// @return True if the lib existed and was erased, false otherwise.
   [[nodiscard]] inline bool remove_lib(const std::string& name) noexcept { return m_libs.erase(name) > 0; }

   /// Remove an object from the project.
   /// @param name  The name of the object to remove.
   /// @return True if the object existed and was erased, false otherwise.
   [[nodiscard]] inline bool remove(const std::string& name) noexcept { return remove_app(name) || remove_lib(name); }

   /// update or insert a new object. It's type() is evaluated to determine which list it belongs in. If the object already
   /// exists.
   /// @param obj  The object to update or insert.
   template <typename Tag>
   inline void upsert(object<Tag>&& obj) noexcept {
      if constexpr (std::is_same_v<Tag, app_tag>)
         return upsert_app(std::move(obj));
      else if constexpr (std::is_same_v<Tag, lib_tag>)
         return upsert_lib(std::move(obj));
      //else if constexpr (Ty == object::type_t::test)
      //   return upsert_test(std::move(obj));
      else
         throw std::runtime_error("internal failure");
   }

   /// update or insert a new application object. If the object already exists, an update is performed by removing the old one and
   /// adding the new one.
   /// @param obj  The object to update or insert.
   void upsert(app_t&& app) noexcept { m_apps[app.name()] = std::move(app); }

   /// update or insert a new library object. If the object already exists, an update is performed by removing the old one and
   /// adding the new one.
   /// @param obj  The object to update or insert.
   void upsert(lib_t&& lib) noexcept { m_libs[lib.name()] = std::move(lib); }

#if 0
   /// update or insert a new test object. If the object already exists, an update is performed by removing the old one and
   /// adding the new one.
   /// @param obj  The object to update or insert.
   void upsert_test(object&& test) noexcept;
#endif

   /// Does the given app exist in the project.
   /// @param name  The app name to search for.
   /// @return true if the app exists, false otherwise.
   [[nodiscard]] inline bool app_exists(const std::string& name) const noexcept { return m_apps.count(name) > 0; }

   /// Does the given lib exist in the project.
   /// @param name  The lib name to search for.
   /// @return true if the lib exists, false otherwise.
   [[nodiscard]] inline bool lib_exists(const std::string& name) const noexcept { return m_libs.count(name) > 0; }

   /// Does the given object exist in the project.
   /// @param name  The object name to search for.
   /// @return true if the object exists, false otherwise.
   [[nodiscard]] inline bool exists(const std::string& name) const noexcept { return app_exists(name) || lib_exists(name); }


   /// Return the first object with the matching name where search order is apps, libs, tests.
   /// @TODO replace this with a std::vector<antler::project::object>/antler::project::object::list_t to return all the objects
   /// with matching names.
   /// @param name  The name to search for in the object lists.
   /// @param type  If type is other than any, the search is limited to that single type.
   /// @return vector with copies of the objects.
   template <typename Tag>
   [[nodiscard]] auto& object(std::string_view name) {
      const auto& get = [](auto nm, auto& c) -> auto& {
         auto itr = c.find(nm);
         return itr->second;
      };

      if constexpr (std::is_same_v<Tag, app_tag>) {
         return get(name, m_apps);
      } else if constexpr (std::is_same_v<Tag, lib_tag>) {
         return get(name, m_libs);
      } else {
         throw std::runtime_error("internal failure");
      }
   } 

   /// @return A const ref to the application list.
   [[nodiscard]] inline const app_t::map_t& apps() const noexcept { return m_apps; }
   /// @return A ref to the application list.
   [[nodiscard]] inline app_t::map_t& apps() noexcept { return m_apps; }
   inline void apps(app_t::map_t&& apps) noexcept { m_apps = std::move(apps); }

   /// @return A const ref to the library list.
   [[nodiscard]] inline const lib_t::map_t& libs() const noexcept { return m_libs; }
   /// @return A ref to the library list.
   [[nodiscard]] inline lib_t::map_t& libs() noexcept { return m_libs; }
   inline void libs(lib_t::map_t&& libs) noexcept { m_libs = std::move(libs); }

   #if 0
   /// @return A const ref to the test list.
   [[nodiscard]] inline const antler::project::object::list_t& tests() const noexcept { return m_tests; }
   /// @return A ref to the test list.
   [[nodiscard]] inline antler::project::object::list_t& tests() noexcept { return m_tests; }
   #endif

   /// Validate a dependency
   /// @param dep Dependency to check
   /// @param error_stream  Stream location for printing warnings and errors.
   /// @return true if the project is valid; otherwise, false.
   [[nodiscard]] bool validate_dependency(const dependency& dep, std::ostream& error_stream = std::cerr) const noexcept;

   /// Validate the project.
   /// @param error_stream  Stream location for printing warnings and errors.
   /// @return true if the project is valid; otherwise, false.
   [[nodiscard]] bool has_valid_dependencies(std::ostream& error_stream = std::cerr) const noexcept;


   /// Populate the directory by generating files.
   /// @param replace  The should replace during population.
   /// @param error_stream  The stream to print failure reports to.
   /// @return true for success; false for failure.
   [[nodiscard]] bool populate(bool replace = true, std::ostream& error_stream = std::cerr) noexcept;

   /// Initialize the directories
   /// @param path  The location of the project.yaml file or the path containing it.
   /// @param error_stream  The stream to print failure reports to.
   /// @return true for success; false indidates failure.
   [[nodiscard]] static bool init_dirs(const std::filesystem::path& path, std::ostream& error_stream = std::cerr) noexcept;

   /// Search this and directories above for `project.yaml` file.
   /// @note if path extension is `.yaml` no directory search is performed, instead return value indicating existence of path a regular file.
   /// @param path  This is the search path to begin with; if the project file was found, it is updated to the path to that file.
   /// @return true if the project file was found and is a regular file; otherwise, false.
   [[nodiscard]] static bool update_path(std::filesystem::path& path) noexcept;

   /// Serialization function from version to yaml node
   [[nodiscard]] inline yaml::node_t to_yaml() const noexcept { 
      yaml::node_t node;
      node["project"] = m_name;
      node["version"] = m_ver;
      for (const auto& [k,v] : apps())
         node["apps"].push_back(v);
      for (const auto& [k,v] : libs())
         node["libs"].push_back(v);
      return node; 
   }

   /// Deserialization function from yaml node to version
   [[nodiscard]] inline bool from_yaml(const yaml::node_t& n) noexcept {
      return ANTLER_EXPECT_YAML(n, "project", name, std::string) &&
             ANTLER_EXPECT_YAML(n, "version", version, antler::project::version) &&
             ANTLER_TRY_YAML_ALL(n, "apps", upsert, antler::project::app_t) &&
             ANTLER_TRY_YAML_ALL(n, "libs", upsert, antler::project::lib_t);
   }

   bool sync() noexcept;

private:

   std::filesystem::path m_path;   ///< path to the project.yaml file.
   std::string m_name;             ///< The project name.
   antler::project::version m_ver; ///< The version information for this project.
   app_t::map_t m_apps;            ///< Map of applications.
   lib_t::map_t m_libs;            ///< Map of libraries.
   //object::list_t m_tests;         ///< List of tests.
};

} // namespace antler::project

ANTLER_YAML_CONVERSIONS(antler::project::project);