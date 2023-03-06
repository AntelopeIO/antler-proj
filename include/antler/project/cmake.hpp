#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.hpp>

#include <filesystem> // path
#include <string_view>


namespace antler::project {

   struct cmake {
      constexpr inline static uint16_t minimum_major = 3;
      constexpr inline static uint16_t minimum_minor = 13;
      constexpr inline static std::string_view cmake_lists = "CMakeLists.txt";
      constexpr inline static std::string_view cmake_exe   = "cmake";

      inline static std::string cmake_target_name(const project& proj, const object& obj) {
         using namespace std::literals;
         return std::string(proj.name()) + "-"s + std::string(obj.name());
      }

      template <typename Stream>
      inline static void emit_add_subdirectory(Stream& s, std::string_view path, std::string_view name) noexcept { 
         s << "add_subdirectory(${CMAKE_SOURCE_DIR}/" << path << "/" << name << ")\n";
         s << std::endl;
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
      inline static void emit_project(Stream& s, const project& proj) noexcept {
         s << "find_package(cdt)\n\n";
         s << "add_subdirectory(${CMAKE_SOURCE_DIR}/../libs ${CMAKE_CURRENT_BINARY_DIR}/libs)\n";
         s << "add_subdirectory(${CMAKE_SOURCE_DIR}/../tests ${CMAKE_CURRENT_BINARY_DIR}/tests)\n";
         s << std::endl;
      }

      template <typename Stream>
      inline static void emit_dependencies(Stream& s, const project& proj, const object& obj) noexcept {
         for (const auto& dep : obj.dependencies()) {
            s << "target_link_libraries(" << cmake_target_name(proj, obj) << " PUBLIC " << dep.name() <<")\n";
         }
         s << std::endl;
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
         s << "add_contract(" << app.name() << " " << cmake_target_name(proj, app) << " " << "${CMAKE_SOURCE_DIR}/../../apps/" 
           << app.name() << "/" << app.name() << ".cpp" << ")\n"; 
         s << "target_include_directories(" << cmake_target_name(proj, app) 
                                            << " PUBLIC ${CMAKE_SOURCE_DIR}/../../include "
                                            << "${CMAKE_SOURCE_DIR}/../../include/" << app.name() << " ./ )\n\n"; 
         emit_dependencies(s, proj, app);
         s << std::endl;
      }

   };

} // namespace antler::project
