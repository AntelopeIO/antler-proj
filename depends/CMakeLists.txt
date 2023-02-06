# @copyright See `LICENSE` in the root directory of this project.

# Set project wide dependencies here.

# Default FetchContent to Verbose. But don't override a user's explicit setting.
if(NOT FETCHCONTENT_QUIET)
  set(FETCHCONTENT_QUIET Off)
endif()

include(FetchContent)
include(ExternalProject)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.24")
  # Set timestamp policy to EXTRACTION time instead of archive time.
  cmake_policy(SET CMP0135 NEW)
endif()

include(./rapidyaml.cmake REQUIRED)
include(./boost.cmake REQUIRED)
