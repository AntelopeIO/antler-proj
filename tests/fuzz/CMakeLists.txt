# @copyright See `LICENSE` in the root directory of this project.

include(CTest)

set(ANTLER_TEST_DIR ${CMAKE_CURRENT_BINARY_DIR})

add_executable(version_fuzz_test
               version_fuzz.cpp
)

set_property(TARGET version_fuzz_test PROPERTY CXX_STANDARD 17)
target_include_directories(version_fuzz_test PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(version_fuzz_test antler-project -fsanitize=address,fuzzer)
target_compile_options(version_fuzz_test PUBLIC -fsanitize=address,fuzzer)