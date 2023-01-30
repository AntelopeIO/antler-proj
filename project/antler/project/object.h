#ifndef antler_project_object_h
#define antler_project_object_h

/// @copyright See `LICENSE` in the root directory of this project.

#include <string>
#include <string_view>
#include <antler/project/dependency.h>
#include <antler/project/language.h>

#include <unordered_set>
#include <vector>

namespace antler {
namespace project {

/// This class represents one of the app, lib, or test objects in a `project.yaml` file.
class object {
public:
   /// This enum is used to communicate object type.
   enum type_t {
      none,                     ///< generally used for invalid state.
      app,                      ///< An application.
      lib,                      ///< A library.
      test,                     ///< A test.
      any,                      ///< any/all, used for searches.
   };
   using list_t = std::vector<object>;

public:
   // use default constructors, copy and move constructors and assignments

   /// Create a n object.
   /// @param ot  The type of this object.
   object(type_t ot);

   /// Object constructor for app and lib types.
   /// @param ot  The type of this object. Must be app or lib.
   /// @param name  The Name of the object.
   /// @param lang  The language type of this object.
   /// @param opts  Compile time options for this object. May be empty.
   object(type_t ot, std::string_view name, antler::project::language lang, std::string_view opts);
   /// Object constructor for test type.
   /// @param name  The Name of the object.
   /// @param command  The command to run for this test.
   object(std::string_view name, std::string_view command);


   /// @return The type of this object.
   type_t type() const noexcept;

   /// @return The project name.
   std::string_view name() const noexcept;
   /// Set the object name.
   void name(std::string_view s) noexcept;


   /// @return Current language.
   antler::project::language language() const noexcept;
   /// Replace any existing language info with the new value.
   /// @param lang  The new language value to store.
   void language(antler::project::language lang) noexcept;

   /// @return Current options.
   std::string_view options() const noexcept;
   /// Replace any existing options with the new value.
   /// @param options  The new options to store.
   void options(std::string_view options) noexcept;

   /// @return  The test command.
   std::string_view command() const noexcept;
   /// @param s  The new test command.
   void command(std::string_view s) noexcept;


   /// Update or insert a dependency.
   /// @param dep  The dependency to upsert.
   void upsert_dependency(antler::project::dependency&& dep) noexcept;
   /// Remove dependency if it exists.
   /// @return true if the dependency was found and removed; otherwise, false (i.e. dependency does not exist)
   bool remove_dependency(std::string_view name) noexcept;
   /// @return The dependency list.
   const antler::project::dependency::list_t& dependencies() const noexcept;
   /// Search the lists to see if a dependency exists.
   /// @param name  The dependency name to search for.
   bool dependency_exists(std::string_view name) const noexcept;
   /// Return the dependency with the matching name.
   /// @param name  The name to search for in the dependency list.
   /// @return optional with a copy of the dependency.
   std::optional<antler::project::dependency> dependency(std::string_view name);

private:
   type_t m_type = none;                               ///< Object type: app, lib, or test.
   std::string m_name;                                 ///< Object name.
   antler::project::dependency::list_t m_dependencies; ///< list of dependencies.

   // app, lib:
   antler::project::language m_language = language::none; ///< Language type, only valid for app or lib.
   std::string m_options;                                 ///< Compile options, only valid for app or lib.

   // test:
   std::string m_command;       ///< Test command, only valid for test.
};

} // namespace project
} // namespace antler


std::ostream& operator<<(std::ostream& os, const antler::project::object::type_t& e);
std::istream& operator>>(std::istream& is, antler::project::object::type_t& e);


#endif
