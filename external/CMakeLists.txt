set(CATCH_INSTALL_DOCS OFF CACHE BOOL "Skip install docs")
set(CATCH_INSTALL_EXTRAS OFF CACHE BOOL "Skip install extras")
set(CATCH_BUILD_TESTING OFF CACHE BOOL "Skip build testing")
add_subdirectory(Catch2)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Catch2/contrib)
include(Catch)

set(JSON_BuildTests OFF CACHE INTERNAL "Skip json tests")
set(JSON_Install OFF CACHE INTERNAL "Skip json install")
add_subdirectory(nlohmann)

set(YAML_CPP_BUILD_CONTRIB OFF CACHE INTERNAL "Skip yaml-cpp contrib")
set(YAML_CPP_BUILD_TOOLS OFF CACHE INTERNAL "Skip yaml-cpp tools")
set(YAML_BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build yaml-cpp static libs")
set(YAML_CPP_INSTALL OFF CACHE INTERNAL "Skip yaml-cpp install")
set(YAML_CPP_FORMAT_SOURCE OFF CACHE INTERNAL "Skip yaml-cpp format source")
add_subdirectory(yaml-cpp)

set(FMT_INSTALL OFF CACHE INTERNAL "Skip fmt install")
add_subdirectory(fmt)

set(CTURTLE_ENABLE_TESTS OFF CACHE INTERNAL "Skip cturtle tests")
set(CTURTLE_ENABLE_INSTALL OFF CACHE INTERNAL "Skip cturtle install")
add_subdirectory(cturtle)
