#ifndef antler_project_project_h
#define antler_project_project_h

#include <antler/project/object.h>
#include <filesystem>
#include <optional>

#include <iostream>


namespace antler {
namespace project {

class project {
public:
   //
   enum class pop {
      force_replace,
      honor_deltas,
      // merge_deltas,
   };

public:
   // parse a project from a project.yml

   // constructors
   project();
   //project(const std::filesystem::path& filename);


   std::string_view name() const noexcept;
   void name(std::string_view s) noexcept;

   std::filesystem::path path() const noexcept;
   void path(const std::filesystem::path& path) noexcept;

   antler::project::version version() const noexcept;
   void version(const antler::project::version& ver) noexcept;


   bool remove(std::string_view name, object::type_t type) noexcept;

   void upsert(object&& obj) noexcept;
   void upsert_app(object&& app) noexcept;
   void upsert_lib(object&& lib) noexcept;
   void upsert_test(object&& test) noexcept;

   /// Search the lists to see if an object exists.
   /// @param name  The object name to search for.
   /// @param type  Limit the search to a single type.
   /// @return true if an object with the provided name exists in the indicated list.
   bool object_exists(std::string_view name, object::type_t type = object::type_t::any) const noexcept;
   /// Return the first object with the matching name where search order is apps, libs, tests.
   /// @param name  The name to search for in the object lists.
   /// @return optional with a copy of the object.
   std::optional<antler::project::object> object(std::string_view name) const noexcept;

   const antler::project::object::list_t& apps() const noexcept;
   const antler::project::object::list_t& libs() const noexcept;
   const antler::project::object::list_t& tests() const noexcept;
   antler::project::object::list_t all_objects() const noexcept;

   /// Validate the project.
   bool is_valid(std::ostream& error_stream = std::cerr);



   /// Print the yaml object to a stream.
   /// @param os  The ostream to print to.
   void print(std::ostream& os) const noexcept;
   /// @return yaml string representation of this object.
   std::string to_yaml() const noexcept;

   /// Write the file to disk.
   /// @note path() must be set.
   /// @param error_stream  The stream to print failure reports to.
   /// @return true for success; false for failure.
   bool sync(std::ostream& error_stream = std::cerr) noexcept;

   /// Populate the directory by generating files.
   /// @param action_type  The type of population action to perform.
   /// @param error_stream  The stream to print failure reports to.
   /// @return true for success; false for failure.
   bool populate(pop action_type, std::ostream& error_stream = std::cerr) noexcept;


   /// Factory function.
   /// @note The returned project may not be valid. The only guarantee is that parsing did not fail.
   /// @note see is_valid() to test validity.
   /// @param path  The location of the project.yaml file or the path containing it.
   /// @param error_stream  The stream to print failure reports to.
   /// @return std::optional containing a project if parsing succeeded.
   static std::optional<project> parse(const std::filesystem::path& path, std::ostream& error_stream = std::cerr);

   /// Initialize the directories
   /// @param path  The location of the project.yaml file or the path containing it.
   /// @param expect_empty  This boolean describes behavior when paths preexist:
   ///                        when true, any existing path - excluding the root - will cause an immediate false return;
   ///                        when false, only failures to create will generate a false return.
   /// @param error_stream  The stream to print failure reports to.
   /// @return true for success; false indidates failure.
   static bool init_dirs(const std::filesystem::path& path, bool expect_empty = true, std::ostream& error_stream = std::cerr) noexcept;

   /// Search this and directories above for `project.yaml` file.
   /// @note if path extension is `.yaml` no directory search is performed, instead return value indicating existence of path a regular file.
   /// @param path  This is the search path to begin with; if the project file was found, it is updated to the path to that file.
   /// @return true if the project file was found and is a regular file; otherwise, false.
   static bool update_path(std::filesystem::path& path) noexcept;

   /// Print the pop enum.
   static void print(std::ostream& os, pop e) noexcept;

private:
   std::filesystem::path m_path; // path to the project.yaml file
   std::string m_name;
   antler::project::version m_ver;
   object::list_t m_apps;
   object::list_t m_libs;
   object::list_t m_tests;
};



} // namespace project
} // namespace antler

inline std::ostream& operator<<(std::ostream& os, const antler::project::project& o) { o.print(os); return os; }
inline std::ostream& operator<<(std::ostream& os, const antler::project::project::pop& e) { antler::project::project::print(os,e); return os; }

#endif
