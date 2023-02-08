/// @copyright See `LICENSE` in the root directory of this project.

#include <cmake.hpp>
#include <sstream>

namespace antler::cmake {

std::string add_subdirectory(const std::filesystem::path& path) noexcept {
   // return std::format("add_subdirectory( {} )\n", path.string());
   std::ostringstream ss;
   ss << "add_subdirectory( " << path << " )\n";
   return ss.str();
}


std::string minimum(unsigned major, unsigned minor, unsigned patch) noexcept {
   /*
   if(!patch) {
      if(!minor)
         return std::format("cmake_minimum_required( {} )\n", major);
      return std::format("cmake_minimum_required( {}.{} )\n", major,minor);
   }
   return std::format("cmake_minimum_required( {}.{}.{} )\n", major,minor,patch);
   */

   std::ostringstream ss;
   ss << "cmake_minimum_required( " << major;
   if (patch)
      ss << "." << minor << "." << patch;
   else if (minor)
      ss << "." << minor;
   ss << " )\n";

   return ss.str();
}

std::string project(std::string_view proj_name) noexcept {
   // return std::format("project( \"{}\" )\n", proj_name);
   std::ostringstream ss;
   ss << "project( \"" << proj_name << "\" )\n";
   return ss.str();
}


std::string project(std::string_view proj_name, const project::semver& ver) noexcept {
   // return std::format("project( \"{}\" VERSION {} )\n", proj_name, ver.string());
   std::ostringstream ss;
   ss << "project( \"" << proj_name << "\" VERSION " << ver << " )\n";
   return ss.str();
}

} // namespace antler::cmake
