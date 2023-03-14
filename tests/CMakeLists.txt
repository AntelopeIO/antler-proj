# @copyright See `LICENSE` in the root directory of this project.

include(CTest)

add_executable(unit_tests
               main.cpp
               cmake_tests.cpp
               init_tests.cpp
               #string_from.cpp
               version_constraint_tests.cpp 
               version_tests.cpp
)

set_property(TARGET unit_tests PROPERTY CXX_STANDARD 17)
target_link_libraries(unit_tests antler-project Catch2::Catch2)
catch_discover_tests(unit_tests)