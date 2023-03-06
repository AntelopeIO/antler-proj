/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.hpp>
#include <antler/project/cmake.hpp>

#include <cstring>
#include <filesystem>
#include <fstream>

namespace antler::project {

namespace { // anonymous

const std::filesystem::path cmake_lists{"CMakeLists.txt"};


/// Test to see if a file has the magic maintenance string.
/// @return true if the file either does NOT exist OR contains the magic
[[nodiscard]] bool has_magic(const std::filesystem::path& path, std::ostream& error_stream = std::cerr) {

   if (!std::filesystem::exists(path))
      return true;

   // search path for magic1.
   std::ifstream ifs(path);
   if (!ifs.is_open()) {
      error_stream << "Failed to open " << path << "\n";
      return false;
   }

   std::array<char, project::magic_comment.size()> buffer{};

   ifs.read(buffer.data(), buffer.size());

   return std::memcmp(buffer.data(), project::magic_comment.data(), buffer.size()) == 0;
}


} // anonymous namespace


bool project::populate(bool replace, std::ostream& error_stream) noexcept {
   // Find the project path, and make sure the subdirs/project directory tree exists.
   auto project_path = m_path.parent_path();
   if (!init_dirs(project_path, false, error_stream)) // expect_empty is `false`, it's okay if everthing exists.
      return false;                                   // But its not okay if the filesystem doesn't already exist AND can't be created.

   auto root_path = project_path / cmake_lists;
   auto apps_path = project_path / std::filesystem::path("apps") / cmake_lists;
   auto libs_path = project_path / std::filesystem::path("libs") / cmake_lists;
   auto tests_path = project_path / std::filesystem::path("tests") / cmake_lists;

   bool create = true;
   // Look to see if the header contains the magic, if it does we will not create the file.
   std::ofstream rfs(root_path);
   std::ofstream afs(apps_path);
   std::ofstream lfs(libs_path);
   std::ofstream tfs(tests_path);

   lfs << "\n";
   lfs.close();

   tfs << "\n";
   tfs.close();

   const auto& try_emit = [&](auto path, auto func) {
      try {
         func();
         return true;
      } catch (const std::exception& e) {
         error_stream << "Error writing to " << path << ": " << e.what() << "\n";
         return false;
      } catch (...) {
         error_stream << "Error writing to " << path << ": UNKNOWN\n";
         return false;
      }
   };

   if (!rfs.good() && !afs.good()) {
      error_stream << "Can not open path for writing\n";
      return false;
   } else {
      return try_emit(root_path, [&]() {
         cmake::emit_preamble(rfs, *this);
         cmake::emit_entry(rfs, *this);
      })
      &&
      try_emit(apps_path, [&]() {
         cmake::emit_preamble(afs, *this);
         cmake::emit_add_base_subdirs(afs);
         cmake::emit_project(afs, *this);
      });
   }

   return false;
}


} // namespace antler::project
