#include <antler/project/cmake.hpp>

namespace antler::project {

km::mustache cmake::add_subdirectory_template  = {"add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/{{path}})\n\n"};
km::mustache cmake::add_subdirectory2_template = {"add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/{{src_path}}\n"
                                                  "                 ${CMAKE_CURRENT_BINARY_DIR}/{{bin_path}})\n\n"};

km::mustache cmake::preamble_template = {"# Generated with {{tool}}, modify at your own risk\n"
                                         "cmake_minimum_required(VERSION {{major}}.{{minor}})\n"
                                         "project(\"{{proj_name}}\" VERSION {{proj_major}}.{{proj_minor}}.{{proj_patch}})\n\n"};

km::mustache cmake::project_stub_template = {"find_package(cdt)\n\n"
                                             "add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libs ${CMAKE_CURRENT_BINARY_DIR}/libs)\n"
                                             "add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../tests ${CMAKE_CURRENT_BINARY_DIR}/tests)\n\n"};

km::mustache cmake::target_compile_template = {"target_compile_options({{target_name}} PUBLIC {{opt}})\n\n"};

km::mustache cmake::target_include_template = {"target_include_directories({{target_name}}\n"
                                               "   PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../../include\n"
                                               "          ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/{{obj_name}}\n"
                                               "          ./)\n\n"};

km::mustache cmake::target_link_libs_template = {"target_link_libraries({{target_name}} PUBLIC {{dep_name}})\n\n"};

km::mustache cmake::entry_template = {"include(ExternalProject)\n"
                                      "if(CDT_ROOT STREQUAL \"\" OR NOT CDT_ROOT)\n"
                                      "   find_package(cdt)\n"
                                      "endif()\n\n"
                                      "ExternalProject_Add(\n"
                                      "   {{proj}}_project\n"
                                      "   SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/apps\n"
                                      "   BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/{{proj}}\n"
                                      "   CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CDT_ROOT}/lib/cmake/cdt/CDTWasmToolchain.cmake\n"
                                      "   UPDATE_COMMAND \"\"\n"
                                      "   PATCH_COMMAND \"\"\n"
                                      "   TEST_COMMAND \"\"\n"
                                      "   INSTALL_COMMAND \"\"\n"
                                      "   BUILD_ALWAYS 1\n"
                                      ")\n\n"};

// Note the dollar_brace value in the following 2 templates. dollar_brace must be set to "${" as a workaround for Mustache decoding the
// wrong pair in "${{{",
km::mustache cmake::add_contract_template = {
   "file(GLOB {{target_name}}-source ${CMAKE_CURRENT_SOURCE_DIR}/../../../apps/{{obj_name}}/*{{source_ext}})\n"
   "add_contract({{obj_name}} {{target_name}} {{dollar_brace}}{{target_name}}-source})\n\n"};
km::mustache cmake::add_library_template = {
   "file(GLOB {{target_name}}-source ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/{{obj_name}}/*{{source_ext}})\n\n"
   "add_library({{target_name}} {{dollar_brace}}{{target_name}}-source})\n\n"};

} // namespace antler::project
