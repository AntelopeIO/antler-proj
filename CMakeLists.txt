# @copyright See `LICENSE` in the root directory of this project.

cmake_minimum_required(VERSION 3.12) # ryml minimum is 3.12

project("ANTLER Project Tools ")

# Add dependent libraries.
#   Make sure this happens BEFORE we include our common settings,
#   otherwise we'll be compiling outside libraries with -Wall,
#   -Wextra, -Werror
add_subdirectory( depends )

include( ./common.cmake REQUIRED )

add_subdirectory( project )
add_subdirectory( aproj )
#add_subdirectory( aproj-qt )


option(BUILD_TESTS "Build and run the tests." On)
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory( test )
endif()
