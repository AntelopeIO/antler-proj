#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/semver.hpp>

#include <filesystem> // path


namespace antler::cmake {

/// @return the cmake_minimum string with trailing newline.
[[nodiscard]] std::string minimum(unsigned major, unsigned minor = 0, unsigned patch = 0) noexcept;

/// @return the add_subdirectory string: add_subdirectory( "<path>" )\n
[[nodiscard]] std::string add_subdirectory(const std::filesystem::path& path) noexcept;

/// @return A string including the project name: project("<proj_name>")\n
[[nodiscard]] std::string project(std::string_view proj_name) noexcept;
/// @return A string including the project name: project("<proj_name>" VERSION <ver>)\n
[[nodiscard]] std::string project(std::string_view proj_name, const project::semver& ver) noexcept;

} // namespace antler::cmake
