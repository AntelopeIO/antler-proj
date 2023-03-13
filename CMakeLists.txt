# @copyright See `LICENSE` in the root directory of this project.

cmake_minimum_required(VERSION 3.11) # If ryml >= 0.5.0 is included via FetchContent or submodule, this minimum is 3.12.

project("ANTLER Project Tools " VERSION 1.0.0)

include( ./common.cmake REQUIRED )

add_subdirectory( external )
add_subdirectory( include )
add_subdirectory( src )
add_subdirectory( tools )


option(PEDANTIC_BUILD "Pedantic builds" On)
option(BUILD_TESTS "Build and run the tests." On)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory( tests )
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
