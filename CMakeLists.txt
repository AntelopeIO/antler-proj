# @copyright See `LICENSE` in the root directory of this project.

cmake_minimum_required(VERSION 3.12) # ryml minimum is 3.12

project("ANTLER Project Tools ")

include( ./common.cmake REQUIRED )

add_subdirectory( project )
add_subdirectory( aproj )


option(BUILD_TESTS "Build and run the tests." On)
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory( test )
endif()
