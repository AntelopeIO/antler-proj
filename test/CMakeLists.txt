# @copyright See `LICENSE` in the root directory of this project.

include(CTest)

add_executable(unit_tests
               #string_from.cpp
               version_constraint.cpp 
               version.cpp
)

set_property(TARGET unit_tests PROPERTY CXX_STANDARD 17)
target_link_libraries(unit_tests PRIVATE antler-project Catch2::Catch2)
catch_discover_tests(unit_tests)