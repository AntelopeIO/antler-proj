# @copyright See `LICENSE` in the root directory of this project.

cmake_minimum_required(VERSION 3.11) # If ryml >= 0.5.0 is included via FetchContent or submodule, this minimum is 3.12.

project("ANTLER Project Tools ")

include( ./common.cmake REQUIRED )

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "11")
    message(FATAL_ERROR "Insufficient clang version")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "10")
    message(FATAL_ERROR "Insufficient gcc version")
  endif()
else()
  message(WARNING "This build requires a C++20 compliant compiler.")
endif()


add_subdirectory( project )
add_subdirectory( aproj )


option(BUILD_TESTS "Build and run the tests." On)
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory( test )
endif()


# Copy licenses to BUILD dir
#
#  NOTE: This is used due to legacy requirements instead of:
#    `install(FILE <license files> COMPONENT Documentation DESTINATION share/aproj/licenses)`
configure_file(LICENSE ${CMAKE_BINARY_DIR}/licenses/LICENSE COPYONLY)
file(GLOB license_files licenses/*)
foreach(full_path IN ITEMS ${license_files})
  string(REPLACE "${CMAKE_SOURCE_DIR}/" "" file_name ${full_path})
  configure_file("${file_name}" "${CMAKE_BINARY_DIR}/${file_name}" COPYONLY)
endforeach()
