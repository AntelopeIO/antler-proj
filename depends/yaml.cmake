

FetchContent_Declare(
  yaml
  URL      https://github.com/yaml/libyaml/archive/refs/tags/0.2.5.tar.gz
  URL_HASH SHA256=fa240dbf262be053f3898006d502d514936c818e422afdcf33921c63bed9bf2e
  PATCH_COMMAND patch -p1 < ${CMAKE_CURRENT_SOURCE_DIR}/patches/yaml.patch  # Disable building test files.
)

FetchContent_MakeAvailable(yaml)
