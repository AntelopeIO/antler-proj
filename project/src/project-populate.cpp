/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.h>
#include <cmake.h>

#include <filesystem>
#include <fstream>

namespace antler {
namespace project {

namespace { // anonymous

const std::filesystem::path cmake_lists{"CMakeLists.txt"};
constexpr std::string_view magic1{"# This file was AUTOGENERATED and is maintained using antler-proj tools."}; // Intentionally missing newline.
constexpr std::string_view magic2{"#   Modification or removal of the above line will cause antler-proj to skip this line."};
constexpr std::string_view magic3{"#   Any changes made to this file will be lost when any antler-proj tool updates this project."};
const std::string magic_all{ std::string(magic1) + "\n" + std::string(magic2) + "\n" + std::string(magic3) + "\n" };

/// Test to see if a file has the magic maintenance string.
/// @return true if the file either does NOT exist OR contains the magic
bool has_magic(const std::filesystem::path& path, std::ostream& error_stream = std::cerr) {

   std::error_code sec;

   if (!std::filesystem::exists(path, sec))
      return true;

   // search path for magic1.
   std::ifstream ifs(path);
   if (!ifs.is_open()) {
      error_stream << "Failed to open " << path << "\n";
      return false;
   }

   for (std::array<char, 255> buffer; ifs.getline(buffer.data(), buffer.size()); /**/) {
      // Sanity check size of search string against the buffer.
      static_assert(magic1.size() < buffer.size(), "Buffer is to small to test for magic value.");
      if (magic1 == buffer.data())
         return true;
   }

   return false;
}


} // anonymous namespace


bool project::populate(pop action_type, std::ostream& error_stream) noexcept {

   bool force_replace = (action_type == pop::force_replace);

   // Sanity check: ensure path is valid.
   if (m_path.empty()) {
      error_stream << "Can not populate a project without a path.\n";
      return false;
   }

   // Find the project path, and make sure the subdirs/project directory tree exists.
   auto project_path = m_path.parent_path();
   if (!init_dirs(project_path, false, error_stream)) // expect_empty is `false`, it's okay if everthing exists.
      return false;                                   // But its not okay if the filesystem doesn't already exist AND can't be created.

   std::error_code sec;

   // Check to see if the top level cmake file needs to be created.
   {
      auto path = project_path / cmake_lists;
      bool create = true;
      if (!force_replace && std::filesystem::exists(path, sec)) {
         // Look to see if the header contains the magic, if it does we will not create the file.
         create = has_magic(path);
      }
      if (create) {
         std::ofstream ofs(path);
         if (!ofs.good()) {
            error_stream << "Can not open path for writing: << " << path << "\n";
         }
         else {
            try {
               ofs
                  << magic_all
                  << "\n"
                  << cmake::minimum(3,11)
                  << "\n"
                  << (m_ver.is_semver() ? cmake::project(m_name) : cmake::project(m_name, static_cast<semver>(m_ver)) )
                  << "\n"
                  << cmake::add_subdirectory("libs")
                  << cmake::add_subdirectory("apps")
                  << "\n"
                  << "option(BUILD_TESTS \"Build and run the tests.\" On)\n"
                  << "if(BUILD_TESTS)\n"
                  << "   enable_testing()\n"
                  << "   " << cmake::add_subdirectory("tests")
                  << "endif()\n"
                  ;
            }
            catch(std::exception& e) {
               error_stream << "Error writing to " << path << ": " << e.what() << "\n";
               return false;
            }
            catch(...) {
               error_stream << "Error writing to " << path << ": UNKNOWN\n";
               return false;
            }
         }
      }
   }



   // At each level we are going to want to create a CMakeLists.txt file and zero or more `.cmake` include files.

   // Each file includes a header indicating the user should not modify it. And if they do, changes will be lost unless they
   // delete this line; however, if they delete the line, auto updates will no longer be possible.

   // Then we update everything.



   // Finally CMake the project. Ensure the output goes to a log file.



   return false;
}


} // namespace project
} // namespace antler
