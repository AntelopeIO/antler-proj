#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/object.hpp>
#include <antler/project/version.hpp>
#include <filesystem>
#include <optional>

#include <iostream>


namespace antler::project {

/// This class represents the contents of a `project.yaml` file. Functions exist to encode and decode from a `project.yaml` file.
class project {
public:
   /// How to populate enum - this enum indicates how a project directory should be populated.
   enum class pop {
      force_replace,            ///< Overwrite any existing files, even the ones the user marked do not change.
      honor_deltas,             ///< Honor do not change markers.
      // merge_deltas,
   };

public:
   // parse a project from a project.yml

   constexpr static inline std::string_view manifest_name = "project.yml";

   // constructors
   project() = default;
   project(const std::filesystem::path&);
   project(std::string_view path, std::string_view name, std::string_view version_raw) :
      m_path(path), m_name(name), m_ver(version_raw) {}

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

   /// Remove a named object of a given type. If type is any, then all objects with name are removed.
   /// @param name  The name of the object(s) to remove.
   /// @param type  The type of the object(s) to remove. If type is any, then all objects with name are removed.
   /// @return True if one or more objects are removed; otherwise, false.
   [[nodiscard]] bool remove(std::string_view name, object::type_t type) noexcept;

   /// update or insert a new object. It's type() is evaluated to determine which list it belongs in. If the object already
   /// exists, an update is performed by removing the old one and adding the new one.
   /// @param obj  The object to update or insert.
   void upsert(object&& obj) noexcept;
   /// update or insert a new application object. If the object already exists, an update is performed by removing the old one and
   /// adding the new one.
   /// @param obj  The object to update or insert.
   void upsert_app(object&& app) noexcept;
   /// update or insert a new library object. If the object already exists, an update is performed by removing the old one and
   /// adding the new one.
   /// @param obj  The object to update or insert.
   void upsert_lib(object&& lib) noexcept;
   /// update or insert a new test object. If the object already exists, an update is performed by removing the old one and
   /// adding the new one.
   /// @param obj  The object to update or insert.
   void upsert_test(object&& test) noexcept;

   /// Search the lists to see if an object exists.
   /// @param name  The object name to search for.
   /// @param type  If type is other than any, the search is limited to that single type.
   /// @return true if an object with the provided name exists in the indicated list.
   [[nodiscard]] bool object_exists(std::string_view name, object::type_t type = object::type_t::any) const noexcept;
   /// Return the first object with the matching name where search order is apps, libs, tests.
   /// @TODO replace this with a std::vector<antler::project::object>/antler::project::object::list_t to return all the objects
   /// with matching names.
   /// @param name  The name to search for in the object lists.
   /// @param type  If type is other than any, the search is limited to that single type.
   /// @return vector with copies of the objects.
   [[nodiscard]] std::vector<antler::project::object> object(std::string_view name, object::type_t type = object::type_t::any) const noexcept;

   /// @return A const ref to the application list.
   [[nodiscard]] const antler::project::object::list_t& apps() const noexcept;
   /// @return A const ref to the library list.
   [[nodiscard]] const antler::project::object::list_t& libs() const noexcept;
   /// @return A const ref to the test list.
   [[nodiscard]] const antler::project::object::list_t& tests() const noexcept;
   /// @return a list of ALL the
   [[nodiscard]] antler::project::object::list_t all_objects() const noexcept;

   /// Validate the project.
   /// @param error_stream  Stream location for printing warnings and errors.
   /// @return true if the project is valid; otherwise, false.
   [[nodiscard]] bool is_valid(std::ostream& error_stream = std::cerr);


   /// Print the yaml object to a stream.
   /// @param os  The ostream to print to.
   void print(std::ostream& os) const noexcept;
   /// @return yaml string representation of this object.
   [[nodiscard]] std::string to_yaml() const noexcept;

   /// Write the file to disk.
   /// @note path() must be set.
   /// @param error_stream  The stream to print failure reports to.
   /// @return true for success; false for failure.
   bool sync(std::ostream& error_stream = std::cerr) noexcept;

   /// Populate the directory by generating files.
   /// @param action_type  The type of population action to perform.
   /// @param error_stream  The stream to print failure reports to.
   /// @return true for success; false for failure.
   [[nodiscard]] bool populate(pop action_type, std::ostream& error_stream = std::cerr) noexcept;


   /// Factory function.
   /// @note The returned project may not be valid. The only guarantee is that parsing did not fail.
   /// @note see is_valid() to test validity.
   /// @param path  The location of the project.yaml file or the path containing it.
   /// @param error_stream  The stream to print failure reports to.
   /// @return std::optional containing a project if parsing succeeded.
   [[nodiscard]] static std::optional<project> parse(const std::filesystem::path& path, std::ostream& error_stream = std::cerr);

   /// Initialize the directories
   /// @param path  The location of the project.yaml file or the path containing it.
   /// @param expect_empty  This boolean describes behavior when paths preexist:
   ///                        when true, any existing path - excluding the root - will cause an immediate false return;
   ///                        when false, only failures to create will generate a false return.
   /// @param error_stream  The stream to print failure reports to.
   /// @return true for success; false indidates failure.
   [[nodiscard]] static bool init_dirs(const std::filesystem::path& path, bool expect_empty = true, std::ostream& error_stream = std::cerr) noexcept;

   /// Search this and directories above for `project.yaml` file.
   /// @note if path extension is `.yaml` no directory search is performed, instead return value indicating existence of path a regular file.
   /// @param path  This is the search path to begin with; if the project file was found, it is updated to the path to that file.
   /// @return true if the project file was found and is a regular file; otherwise, false.
   [[nodiscard]] static bool update_path(std::filesystem::path& path) noexcept;

   /// Print the pop enum.
   /// @param os  The output stream to print to.
   /// @param e  The pop enum to print.
   static void print(std::ostream& os, pop e) noexcept;

private:
   std::filesystem::path m_path;   ///< path to the project.yaml file.
   std::string m_name;             ///< The project name.
   antler::project::version m_ver; ///< The version information for this project.
   object::list_t m_apps;          ///< List of applications.
   object::list_t m_libs;          ///< List of libraries.
   object::list_t m_tests;         ///< List of tests.
};

} // namespace antler::project


inline std::ostream& operator<<(std::ostream& os, const antler::project::project& o) { o.print(os); return os; }
inline std::ostream& operator<<(std::ostream& os, const antler::project::project::pop& e) { antler::project::project::print(os,e); return os; }
