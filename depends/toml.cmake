

set(TOML_GIT_HASH 894902820a3ea2f1ec470cd7fe338bde54045cf5)
FetchContent_Declare(
  toml
  URL      https://github.com/cktan/tomlc99/archive/${TOML_GIT_HASH}.tar.gz
  URL_HASH SHA256=3ae1970c4f2d03ac16f8616963dfa6d1fac38883408f76fefbecfc1d8f61e570
  PATCH_COMMAND patch -p1 < ${CMAKE_CURRENT_SOURCE_DIR}/patches/toml.patch
)

FetchContent_MakeAvailable(toml)
