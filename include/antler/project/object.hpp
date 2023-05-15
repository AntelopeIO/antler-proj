#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include <regex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "dependency.hpp"

namespace antler::project {

/// This class represents one of the app, lib, or test objects in a `project.yaml` file.
template <typename Tag>
class object {
public:
   using list_t         = std::vector<object>;
   using map_t          = std::unordered_map<std::string, object>;
   using dependencies_t = std::unordered_map<std::string, dependency>;
   using tag_t          = Tag;

public:
   /// Object constructor for app and lib types.
   object() = default;

   /// Object constructor for app and lib types.
   /// @param name  The Name of the object.
   /// @param lang  The language type of this object.
   /// @param copts  Compile time options for this object. May be empty.
   /// @param lopts  Compile time options for this object. May be empty.
   object(std::string_view name, const std::string& lang, std::string_view copts, std::string_view lopts)
      : m_name{name}
      , m_language{system::language_spec(lang)}
      , m_comp_options{system::split<';'>(copts)}
      , m_link_options{system::split<';'>(lopts)} {
      system::debug_log("object::object(name, lang, copts, lopts) created");
      system::debug_log("with name: {0} lang: {1} copts: {2} lopts: {3}", name, lang, copts, lopts);
   }


   object(const object&) = default;

   object& operator=(const object&) = default;
   object& operator=(object&&)      = default;

   /// @return The project name.
   [[nodiscard]] inline const std::string& name() const noexcept { return m_name; }

   /// Set the object name.
   inline void name(std::string_view n) noexcept { m_name = n; }


   /// @return Current language.
   [[nodiscard]] inline std::string_view language() const noexcept { return m_language; }

   /// Replace any existing language info with the new value.
   /// @param lang  The new language value to store.
   inline void language(std::string_view lang) noexcept { m_language = system::language_spec(lang); }

   /// @return Current compile options.
   [[nodiscard]] inline const std::vector<std::string>& compile_options() const noexcept { return m_comp_options; }
   /// @return Current link options.
   [[nodiscard]] inline const std::vector<std::string>& link_options() const noexcept { return m_link_options; }

   /// Replace any existing options with the new value.
   /// @param options  The new options to store.
   inline void compile_options(std::string_view options) noexcept { m_comp_options = system::split<';'>(options); }

   /// Replace any existing options with the new value.
   /// @param options  The new options to store.
   inline void compile_options(std::vector<std::string> options) noexcept { m_comp_options = std::move(options); }

   /// Add a compile option.
   /// @param option  The new option to store.
   inline void add_compile_option(const std::string& option) noexcept { m_comp_options.push_back(option); }

   /// Replace any existing options with the new value.
   /// @param options  The new options to store.
   inline void link_options(std::string_view options) noexcept { m_link_options = system::split<';'>(options); }

   /// Replace any existing options with the new value.
   /// @param options  The new options to store.
   inline void link_options(const std::vector<std::string> options) noexcept { m_link_options = std::move(options); }

   /// Add a link option.
   /// @param option  The new option to store.
   inline void add_link_option(const std::string& option) noexcept { m_link_options.push_back(option); }

   /// Update or insert a dependency.
   /// @param dep  The dependency to upsert.
   /// @return false if it is an insert, true if it is an update
   bool upsert_dependency(antler::project::dependency&& dep) noexcept {
      const auto& itr       = m_dependencies.find(dep.name());
      bool        has_value = itr != m_dependencies.end();

      if (has_value) {
         itr->second = dep;
      } else {
         auto name = dep.name();
         m_dependencies.emplace(std::move(name), std::move(dep));
      }

      return has_value;
   }

   /// Remove dependency if it exists.
   /// @return true if the dependency was found and removed; otherwise, false (i.e. dependency does not exist)
   inline bool remove_dependency(const std::string& name) noexcept { return m_dependencies.erase(name) > 0; }

   /// @return The dependency map.
   [[nodiscard]] inline const dependencies_t& dependencies() const noexcept { return m_dependencies; }

   /// @return The dependency map.
   [[nodiscard]] inline dependencies_t& dependencies() noexcept { return m_dependencies; }

   /// Update the map of dependencies.
   /// @param deps The map of dependencies to set.
   inline void dependencies(dependencies_t&& deps) noexcept { m_dependencies = std::move(deps); }

   /// Search the lists to see if a dependency exists.
   /// @param name  The dependency name to search for.
   [[nodiscard]] inline bool dependency_exists(const std::string& name) const noexcept { return m_dependencies.count(name) > 0; }

   /// Return the dependency with the matching name.
   /// @param name  The name to search for in the dependency list.
   /// @return optional with a copy of the dependency.
   [[nodiscard]] std::optional<antler::project::dependency> find_dependency(const std::string& name) {
      auto itr = m_dependencies.find(name);
      if (itr == m_dependencies.end())
         return std::nullopt;
      return itr->second;
   }

   /// If an object name is valid? I.e. valid C/C++ name.
   /// @param name  The name to check.
   /// @return true if the name is valid, false otherwise.
   [[nodiscard]] inline static bool is_valid_name(std::string_view name) {
      return !name.empty() && std::regex_match(name.data(), std::regex("[a-zA-z][_a-zA-Z0-9]*"));
   }

   /// Serialization function from version to yaml node
   [[nodiscard]] inline yaml::node_t to_yaml() const noexcept {
      // conjoin all the strings with a ';' between each
      const auto& to_str = [&](const auto& opts) {
         std::string ret = {};
         for (auto itr = opts.begin(); itr != opts.end(); ++itr) {
            ret += *itr;
            if (itr != opts.end() - 1)
               ret += ";";
         }
         return ret;
      };

      yaml::node_t n;
      // TODO we will need to readdress when adding support for tests
      n["name"]            = m_name;
      n["lang"]            = m_language;
      n["compile_options"] = to_str(compile_options());
      n["link_options"]    = to_str(link_options());
      for (const auto& [k, v] : dependencies()) {
         n["depends"].push_back(v);
      }
      return n;
   }

   /// Deserialization function from yaml node to version
   [[nodiscard]] inline bool from_yaml(const yaml::node_t& n) noexcept {
      return ANTLER_EXPECT_YAML(n, "name", name, std::string) &&
             ANTLER_EXPECT_YAML(n, "lang", language, std::string) &&
             ANTLER_TRY_YAML(n, "compile_options", compile_options, std::string) &&
             ANTLER_TRY_YAML(n, "link_options", link_options, std::string) &&
             ANTLER_TRY_YAML_ALL(n, "depends", upsert_dependency, antler::project::dependency);
   }

private:
   std::string    m_name = "";     ///< Object name.
   dependencies_t m_dependencies;  ///< map of dependencies

   std::string              m_language     = "";  ///< Language type, only valid for app or lib.
   std::vector<std::string> m_comp_options = {};  ///< compile options for the object
   std::vector<std::string> m_link_options = {};  ///< linker options for the object
};

struct app_tag;
using app_t = object<app_tag>;

struct lib_tag;
using lib_t = object<lib_tag>;
}  // namespace antler::project


namespace std {
template <typename Tag>
struct hash<antler::project::object<Tag>> {
   std::size_t operator()(const antler::project::object<Tag>& k) const {
      return std::hash<std::string>{}(k.name());
   }
};
}  // namespace std

ANTLER_YAML_CONVERSIONS(antler::project::app_t);
ANTLER_YAML_CONVERSIONS(antler::project::lib_t);
