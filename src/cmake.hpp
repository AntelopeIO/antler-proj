#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.hpp>

#include <filesystem> // path


namespace antler {

   struct cmake {
      template <typename Stream>
      inline void emit_version(Stream& s) const noexcept { s << major << "." << minor; }

      template <typename Stream>
      inline void emit_add_subdirectory(Stream& s, const std::filesystem::path& p) const noexcept { 
         s << "add_subdirectory(" << p.string() << ")\n";
      }

      template <typename Stream>
      inline void emit_preamble(Stream& s, const project::project& proj) const noexcept {
         s << "# Generated with antler-proj tool, modify at your own risk\n";
         s << "cmake_minimum_required(VERSION" << major << "." << minor << ")\n";
         s << "project(\"" << proj.name() << "\" VERSION" << proj.version().major() << "." << 
                              proj.version().minor() << "." << proj.version().patch() << ")" << std::endl;
      }

      uint16_t major = 3;
      uint16_t minor = 11;
   };

/// @return the cmake_minimum string with trailing newline.
[[nodiscard]] std::string minimum(unsigned major = 0, unsigned minor = 0, unsigned patch = 0) noexcept;

/// @return the add_subdirectory string: add_subdirectory( "<path>" )\n
[[nodiscard]] std::string add_subdirectory(const std::filesystem::path& path) noexcept;

/// @return A string including the project name: project("<proj_name>")\n
//[[nodiscard]] std::string project(std::string_view proj_name);
/// @return A string including the project name: project("<proj_name>" VERSION <ver>)\n
//[[nodiscard]] std::string project(std::string_view proj_name, const project::semver& ver) noexcept;

} // namespace antler
