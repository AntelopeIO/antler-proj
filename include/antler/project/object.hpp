#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include <regex>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "dependency.hpp"

namespace antler::project {

/// This class represents one of the app, lib, or test objects in a `project.yaml` file.
class object {
public:
   enum class type_t : uint8_t {
      app,
      lib,
      test,
      any,
      error
   };
   using list_t = std::vector<object>;

public:
   // use default constructors, copy and move constructors and assignments

   /// Create a n object.
   /// @param ot  The type of this object.
   explicit object(type_t ot);

   /// Object constructor for app and lib types.
   /// @param ot  The type of this object. Must be app or lib.
   /// @param name  The Name of the object.
   /// @param lang  The language type of this object.
   /// @param copts  Compile time options for this object. May be empty.
   /// @param lopts  Compile time options for this object. May be empty.
   object(type_t ot, std::string_view name, const std::string& lang, std::string_view copts, std::string_view lopts);
   /// Object constructor for test type.
   /// @param name  The Name of the object.
   /// @param command  The command to run for this test.
   object(std::string_view name, std::string_view command);

   /// @return The type of this object.
   [[nodiscard]] type_t type() const noexcept;

   /// @return The project name.
   [[nodiscard]] std::string_view name() const noexcept;
   /// Set the object name.
   void name(std::string_view s) noexcept;


   /// @return Current language.
   [[nodiscard]] std::string_view language() const noexcept;
   /// Replace any existing language info with the new value.
   /// @param lang  The new language value to store.
   void language(std::string_view lang) noexcept;

   /// @return Current compile options.
   [[nodiscard]] inline std::string_view compile_options() const noexcept { return m_comp_options; }
   /// @return Current link options.
   [[nodiscard]] inline std::string_view link_options() const noexcept { return m_link_options; }

   /// Replace any existing options with the new value.
   /// @param options  The new options to store.
   void compile_options(std::string_view options) noexcept { m_comp_options = options; }

   /// Replace any existing options with the new value.
   /// @param options  The new options to store.
   void link_options(std::string_view options) noexcept { m_link_options = options; }

   /// @return  The test command.
   [[nodiscard]] std::string_view command() const noexcept;
   /// @param s  The new test command.
   void command(std::string_view s) noexcept;

   /// Update or insert a dependency.
   /// @param dep  The dependency to upsert.
   void upsert_dependency(antler::project::dependency&& dep) noexcept;

   /// Remove dependency if it exists.
   /// @return true if the dependency was found and removed; otherwise, false (i.e. dependency does not exist)
   bool remove_dependency(std::string_view name) noexcept;
   /// @return The dependency list.
   [[nodiscard]] const antler::project::dependency::list_t& dependencies() const noexcept;
   /// Search the lists to see if a dependency exists.
   /// @param name  The dependency name to search for.
   [[nodiscard]] bool dependency_exists(std::string_view name) const noexcept;
   /// Return the dependency with the matching name.
   /// @param name  The name to search for in the dependency list.
   /// @return optional with a copy of the dependency.
   [[nodiscard]] std::optional<antler::project::dependency> dependency(std::string_view name);

   /// If an object name is valid? I.e. valid C/C++ name.
   /// @param name  The name to check.
   /// @return true if the name is valid, false otherwise.
   [[nodiscard]] inline static bool is_valid_name(std::string_view name) { 
      return std::regex_match(name.data(), std::regex("[a-zA-z][_a-zA-Z0-9]+")); 
   }

private:
   type_t m_type = type_t::error;                      ///< Object type: app, lib, or test.
   std::string m_name = "";                            ///< Object name.
   antler::project::dependency::list_t m_dependencies; ///< list of dependencies.

   // app, lib:
   std::string m_language = ""; ///< Language type, only valid for app or lib.
   std::string m_comp_options = "";
   std::string m_link_options = "";

   // test:
   std::string m_command  = "";       ///< Test command, only valid for test.
};

} // namespace antler::project