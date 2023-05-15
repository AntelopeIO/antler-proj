#include <antler/project/cmake.hpp>

namespace antler::project {

// 0: path
std::string_view cmake::add_subdirectory_template = {"add_subdirectory(${{CMAKE_CURRENT_SOURCE_DIR}}/{0})\n\n"};
// 0: src_path, 1: bin_path
std::string_view cmake::add_subdirectory2_template = {
   "add_subdirectory(${{CMAKE_CURRENT_SOURCE_DIR}}/{0}\n"
   "                 ${{CMAKE_CURRENT_BINARY_DIR}}/{1})\n\n"};

// 0: tool, 1: major, 2: minor, 3: proj_name, 4: proj_major, 5: proj_minor, 6: proj_patch
std::string_view cmake::preamble_template = {
   "# Generated with {0}, modify at your own risk\n"
   "cmake_minimum_required(VERSION {1}.{2})\n"
   "project(\"{3}\" VERSION {4}.{5}.{6})\n\n"};

// NONE, but we still need to send it through format.
std::string_view cmake::project_stub_template = {
   "find_package(cdt)\n\n"
   "add_subdirectory(${{CMAKE_CURRENT_SOURCE_DIR}}/../libs ${{CMAKE_CURRENT_BINARY_DIR}}/libs)\n"
   "add_subdirectory(${{CMAKE_CURRENT_SOURCE_DIR}}/../tests ${{CMAKE_CURRENT_BINARY_DIR}}/tests)\n\n"};

// 0: target_name, 1: opt
std::string_view cmake::target_compile_template = {"target_compile_options({0} PUBLIC {1})\n\n"};

// 0: target name, 1: obj name
std::string_view cmake::target_include_template = {
   "target_include_directories({0}\n"
   "   PUBLIC ${{CMAKE_CURRENT_SOURCE_DIR}}/../../../include\n"
   "          ${{CMAKE_CURRENT_SOURCE_DIR}}/../../../include/{1}\n"
   "          ./)\n\n"};

// 0: target name, 1: link libs
std::string_view cmake::target_link_libs_template = {"target_link_libraries({0} PUBLIC {1})\n\n"};

// 0: proj
std::string_view cmake::entry_template = {
   "include(ExternalProject)\n"
   "if(CDT_ROOT STREQUAL \"\" OR NOT CDT_ROOT)\n"
   "   find_package(cdt)\n"
   "endif()\n\n"
   "ExternalProject_Add(\n"
   "   {0}_project\n"
   "   SOURCE_DIR ${{CMAKE_CURRENT_SOURCE_DIR}}/apps\n"
   "   BINARY_DIR ${{CMAKE_CURRENT_BINARY_DIR}}/{0}\n"
   "   CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${{CDT_ROOT}}/lib/cmake/cdt/CDTWasmToolchain.cmake\n"
   "   UPDATE_COMMAND \"\"\n"
   "   PATCH_COMMAND \"\"\n"
   "   TEST_COMMAND \"\"\n"
   "   INSTALL_COMMAND \"\"\n"
   "   BUILD_ALWAYS 1\n"
   ")\n\n"};

// 0: object name, 1: target name, 2: source extension
std::string_view cmake::add_contract_template = {
   "file(GLOB {1}-source ${{CMAKE_CURRENT_SOURCE_DIR}}/../../../apps/{0}/*{2})\n"
   "add_library({1} STATIC ${{{1}-source}})\n"
   "target_compile_options({1} PRIVATE -Wno-unused-command-line-argument)\n"
   "add_executable({1}-app ${{{1}-source}})\n"
   "get_target_property({1}-BINOUTPUT {1}-app BINARY_DIR)\n"
   "target_compile_options({1}-app PUBLIC -contract {0})\n"
   "target_compile_options({1}-app PUBLIC -abigen)\n"
   "target_compile_options({1}-app PUBLIC -abigen_output=${{{1}-BINOUTPUT}}/{0}.abi)\n"
   "set_target_properties({1}-app PROPERTIES OUTPUT_NAME {0})\n"
   "target_include_directories({1}-app PUBLIC $<TARGET_PROPERTY:{1},INTERFACE_INCLUDE_DIRECTORIES>)\n"
   "target_link_libraries({1}-app PUBLIC $<TARGET_PROPERTY:{1},LINK_LIBRARIES>)\n"
   "target_compile_options({1}-app PUBLIC $<TARGET_PROPERTY:{1},COMPILE_OPTIONS>)\n\n"};

// 0: object name, 1: target name, 2: source extension, 3: location
std::string_view cmake::add_library_template = {
   "file(GLOB {1}-source ${{CMAKE_CURRENT_SOURCE_DIR}}/../../../libs/{0}/*{2})\n\n"
   "add_library({1} STATIC ${{{1}-source}})\n\n"};

}  // namespace antler::project
