# rapidyaml (ryml) dependency
# @copyright See `LICENSE` in the root directory of this project.

FetchContent_Declare(
  ryml
  SYSTEM
  URL      https://github.com/biojppm/rapidyaml/releases/download/v0.5.0/rapidyaml-0.5.0-src.tgz
  URL_HASH SHA256=6493557778791a3a2375510ce6c0ecd70163fc8ce4f8ed683acc36e3e55ee881
  # Patch that allows for:
  #   `operator std::string_view()`
  #   C++20 standard
  PATCH_COMMAND patch -p1 < ${CMAKE_CURRENT_SOURCE_DIR}/patches/rapidyaml.patch
)

FetchContent_MakeAvailable(ryml)
