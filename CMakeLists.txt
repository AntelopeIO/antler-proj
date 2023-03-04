# @copyright See `LICENSE` in the root directory of this project.

cmake_minimum_required(VERSION 3.11) # If ryml >= 0.5.0 is included via FetchContent or submodule, this minimum is 3.12.

project("ANTLER Project Tools " VERSION 1.0.0)

include( ./common.cmake REQUIRED )

add_subdirectory( external )
add_subdirectory( include )
add_subdirectory( src )
add_subdirectory( tools )


option(PEDANTIC_BUILD "Pedantic builds" On)
option(BUILD_TESTS "Build and run the tests." Off)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory( test )
endif()
