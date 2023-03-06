#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.hpp>

#include <filesystem> // path
#include <string_view>


namespace antler::project {

   struct cmake {
      constexpr inline static uint16_t minimum_major = 3;
      constexpr inline static uint16_t minimum_minor = 13;

      inline static std::string cmake_target_name(const project& proj, const object& obj) {
         using namespace std::literals;
         return std::string(proj.name()) + "_"s + std::string(obj.name());
      }

      template <typename Stream>
      inline static void emit_add_subdirectory(Stream& s, const std::filesystem::path& p) noexcept { 
         s << "add_subdirectory(" << p.string() << ")\n";
      }

      template <typename Stream>
      inline static void emit_preamble(Stream& s, const project& proj) noexcept {
         s << "# Generated with antler-proj tool, modify at your own risk\n";
         s << "cmake_minimum_required(VERSION " << minimum_major << "." << minimum_minor << ")\n";
         s << "project(\"" << proj.name() << "\" VERSION " << proj.version().major() << "." << 
                              proj.version().minor() << "." << proj.version().patch() << ")\n";
         s << std::endl;
      }

      template <typename Stream>
      inline static void emit_add_base_subdirs(Stream& s) noexcept {
         s << "add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libs ${CMAKE_CURRENT_BINARY_DIR}/libs)\n";
         s << "add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../tests ${CMAKE_CURRENT_BINARY_DIR}/tests)\n";
         s << std::endl;
      }

      template <typename Stream>
      inline static void emit_dependencies(Stream& s, const project& proj, const object& obj) noexcept {
         for (const auto& dep : obj.dependencies()) {
            s << "target_link_libraries(" << cmake_target_name(proj, obj) << " PUBLIC " << dep.name() <<")\n";
         }
      }

      template <typename Stream>
      inline static void emit_entry(Stream& s, const project& proj) noexcept {
         s << "include(ExternalProject)\n";
         s << "if(CDT_ROOT STREQUAL \"\" OR NOT CDT_ROOT)\n";
         s << "\tfind_package(cdt)\n";
         s << "endif()\n\n";
         s << "ExternalProject_Add(\n";
         s << "\t" << proj.name() << "_project\n";
         s << "\tSOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/apps\n";
         s << "\tBINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/" << proj.name() << "\n";
         s << "\tCMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CDT_ROOT}/lib/cmake/cdt/CDTWasmToolchain.cmake\n";
         s << "\tUPDATE_COMMAND \"\"\n";
         s << "\tPATCH_COMMAND \"\"\n";
         s << "\tTEST_COMMAND \"\"\n";
         s << "\tINSTALL_COMMAND \"\"\n";
         s << "\tBUILD_ALWAYS 1\n";
         s << ")\n";
         s << std::endl;
      }

      template <typename Stream>
      inline static void emit_app(Stream& s, const object& app, const project& proj) noexcept {
         s << "add_executable(" << cmake_target_name(proj, app) << " " << app.name() << ".cpp" << ")\n"; 
         s << "target_include_directories(" << cmake_target_name(proj, app) 
                                            << " PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include "
                                            << "${CMAKE_CURRENT_SOURCE_DIR}/include/" << app.name() << " ./ )\n\n"; 
         emit_dependencies(s, proj, app);
         s << std::endl;
      }

      template <typename Stream>
      inline static void emit_project(Stream& s, const project& proj) noexcept {
         for (auto app : proj.apps()) {
            emit_app(s, app, proj);
         }
      }
   };

/// @return the cmake_minimum string with trailing newline.
[[nodiscard]] std::string minimum(unsigned major = 0, unsigned minor = 0, unsigned patch = 0) noexcept;

/// @return the add_subdirectory string: add_subdirectory( "<path>" )\n
[[nodiscard]] std::string add_subdirectory(const std::filesystem::path& path) noexcept;

/// @return A string including the project name: project("<proj_name>")\n
//[[nodiscard]] std::string project(std::string_view proj_name);
/// @return A string including the project name: project("<proj_name>" VERSION <ver>)\n
//[[nodiscard]] std::string project(std::string_view proj_name, const project::semver& ver) noexcept;

} // namespace antler::project
