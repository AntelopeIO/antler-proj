# @copyright See `LICENSE` in the root directory of this project.

cmake_minimum_required(VERSION 3.10)

project("ANTLER Project Tools " VERSION 1.0.0)

include( ./common.cmake REQUIRED )



option(ANTLER_PROJ_PEDANTIC_BUILD "Pedantic builds" On)
option(ANTLER_PROJ_BUILD_TESTS "Build and run the tests." On)
option(ANTLER_PROJ_TIDY_CHECKS "Uses clang-tidy in building." On)

add_subdirectory( external )
add_subdirectory( include )
add_subdirectory( src )
add_subdirectory( tools )

if(ANTLER_PROJ_BUILD_TESTS)
  enable_testing()
  add_subdirectory( tests )
endif()

# Clang-tidy support
if (ANTLER_PROJ_TIDY_CHECKS)
  find_program(CLANG_TIDY_EXE NAMES "clang-tidy" REQUIRED)
  set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "-checks=*,modernize-*")
endif()

# Copy licenses to BUILD dir
#
#  NOTE: This is used due to legacy requirements instead of:
#    `install(FILE <license files> COMPONENT Documentation DESTINATION share/aproj/licenses)`
configure_file(LICENSE licenses/LICENSE COPYONLY)
file(GLOB license_files licenses/*)
foreach(full_path IN ITEMS ${license_files})
   string(REPLACE "${CMAKE_SOURCE_DIR}/" "" file_name ${full_path})
   configure_file("${full_path}" "${file_name}" COPYONLY)
endforeach()
