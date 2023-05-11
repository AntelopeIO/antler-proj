/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.hpp>
#include <antler/project/cmake.hpp>

#include <cstring>
#include <filesystem>
#include <fstream>

namespace antler::project {

namespace { // anonymous



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


bool project::populate(bool, std::ostream& error_stream) noexcept {
   // Find the project path, and make sure the subdirs/project directory tree exists.
   auto project_path = m_path.parent_path();
   if (!init_dirs(project_path, error_stream)) // expect_empty is `false`, it's okay if everthing exists.
      return false;                                   // But its not okay if the filesystem doesn't already exist AND can't be created.
   
   auto build_path = project_path / std::filesystem::path("build");
   std::filesystem::create_directory(build_path);
   std::filesystem::create_directory(project_path / std::filesystem::path("build") / std::filesystem::path("apps"));
   std::filesystem::create_directory(project_path / std::filesystem::path("build") / std::filesystem::path("libs"));
   std::filesystem::create_directory(project_path / std::filesystem::path("build") / std::filesystem::path("tests"));

   auto root_path = build_path / cmake::cmake_lists;
   auto apps_path = build_path / std::filesystem::path("apps") / cmake::cmake_lists;
   auto libs_path = build_path / std::filesystem::path("libs") / cmake::cmake_lists;
   auto tests_path = build_path / std::filesystem::path("tests") / cmake::cmake_lists;

   // Look to see if the header contains the magic, if it does we will not create the file.
   std::ofstream rfs(root_path);
   std::ofstream afs(apps_path);
   std::ofstream lfs(libs_path);
   std::ofstream tfs(tests_path);

   tfs << "\n";

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
      if (!try_emit(root_path, [&]() {
         cmake::emit_preamble(rfs, *this);
         cmake::emit_entry(rfs, *this);
      })) {
         error_stream << "Error emitting cmake for root.\n";
         return false;
      }
      
      if (!try_emit(apps_path, [&]() {
         cmake::emit_preamble(afs, *this);
         cmake::emit_project(afs, *this);
      })) {
         error_stream << "Error emitting base cmake for project.\n";
         return false;
      }
      
      for (const auto& app : apps()) {
         auto app_path = apps_path.parent_path() / std::filesystem::path(app.name()) / cmake::cmake_lists;
         std::filesystem::create_directory(app_path.parent_path());
         std::ofstream apfs(app_path);

         if (!try_emit(apps_path, [&](){
            cmake::emit_add_subdirectory(afs, ".", app.name());
         })) {
            error_stream << "Error emitting cmake for app: " << app.name() << "\n";
            return false;
         }

         if (!try_emit(app_path, [&]() {
            cmake::emit_app(apfs, app, *this);
         })) {
            error_stream << "Error emitting cmake for app: " << app.name() << "\n";
            return false;
         }

	 if (!try_emit(app_path, [&]() {
            cmake::emit_lib(apfs, app, *this);
         })) {
            error_stream << "Error emitting cmake for app-lib: " << app.name() << "\n";
            return false;
         }
      }

      for (const auto& lib : libs()) {
         auto lib_path = libs_path.parent_path() / std::filesystem::path(lib.name()) / cmake::cmake_lists;
         std::filesystem::create_directory(lib_path.parent_path());
         std::ofstream lpfs(lib_path);

         if (!try_emit(libs_path, [&](){
            cmake::emit_add_subdirectory(lfs, "../libs", lib.name());
         })) {
            error_stream << "Error emitting cmake for lib: " << lib.name() << "\n";
            return false;
         }

         if (!try_emit(lib_path, [&]() {
            cmake::emit_lib(lpfs, lib, *this);
         })) {
            error_stream << "Error emitting cmake for lib: " << lib.name() << "\n";
            return false;
         }
      }

   }

   return true;
}


} // namespace antler::project
