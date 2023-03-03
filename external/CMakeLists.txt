add_subdirectory(rapidyaml)

set(MAGIC_ENUM_OPT_BUILD_EXAMPLES OFF CACHE BOOL "Skip magic_enum examples")
set(MAGIC_ENUM_OPT_BUILD_TESTS OFF CACHE BOOL "Skip magic_enum tests")
set(MAGIC_ENUM_OPT_INSTALL OFF CACHE BOOL "Skip magic_enum install")
add_subdirectory(magic_enum)