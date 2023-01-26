
include(FetchContent)
include(ExternalProject)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.24")
  # Set timestamp policy to EXTRACTION time instead of archive time.
  cmake_policy(SET CMP0135 NEW)
endif()

include(./rapidyaml.cmake REQUIRED)
#include(./yaml-cpp.cmake REQUIRED)
#include(./yaml.cmake REQUIRED)
#include(./toml.cmake REQUIRED)
