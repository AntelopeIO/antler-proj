#ifndef antler_cmake_h
#define antler_cmake_h

#include <antler/project/semver.h>

#include <filesystem> // path


namespace antler {
namespace cmake {

/// @return the cmake_minimum string with trailing newline.
std::string minimum(unsigned major, unsigned minor = 0, unsigned patch = 0) noexcept;

std::string add_subdirectory(const std::filesystem::path path) noexcept;

/// @return A string including the project name: project("<proj_name>")
std::string project(std::string_view proj_name) noexcept;
/// @return A string including the project name: project("<proj_name>" VERSION <ver>)
std::string project(std::string_view proj_name, const project::semver& ver) noexcept;

} // namespace cmake
} // namespace antler

#endif
