

FetchContent_Declare(
  yaml-cpp
  SYSTEM
  URL      https://github.com/jbeder/yaml-cpp/archive/refs/tags/yaml-cpp-0.7.0.tar.gz
  URL_HASH SHA256=43e6a9fcb146ad871515f0d0873947e5d497a1c9c60c58cb102a97b47208b7c3
  #PATCH_COMMAND patch -p1 < ${CMAKE_CURRENT_SOURCE_DIR}/patches/yaml.patch  # Disable building test files.
)

FetchContent_MakeAvailable(yaml-cpp)
