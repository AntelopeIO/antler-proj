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
