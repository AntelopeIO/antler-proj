#ifndef antler_project_object_h
#define antler_project_object_h

#include <string>
#include <string_view>
#include <antler/project/dependency.h>
#include <antler/project/language.h>

#include <unordered_set>
#include <vector>

namespace antler {
namespace project {

class object {
public:
   enum type_t {
      none,
      app,
      lib,
      test,
      any,                      // any/all
   };
   using list_t = std::vector<object>;

public:
   // use default constructors, copy and move constructors and assignments
   object(type_t ot);

   /// Object constructor for app and lib types.
   object(type_t ot, std::string_view name, antler::project::language lang, std::string_view opts);
   /// Object constructor for test type.
   object(std::string_view test_name, std::string_view command);


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

   std::string_view command() const noexcept;
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
   type_t m_type = none;
   std::string m_name;
   antler::project::dependency::list_t m_dependencies;

   // app, lib:
   antler::project::language m_language = language::none;
   std::string m_options;

   // test:
   std::string m_command;
};


} // namespace project
} // namespace antler

std::ostream& operator<<(std::ostream& os, const antler::project::object::type_t& e);
std::istream& operator>>(std::istream& is, antler::project::object::type_t& e);


#endif
