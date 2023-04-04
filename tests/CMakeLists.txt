# @copyright See `LICENSE` in the root directory of this project.

include(CTest)

set(ANTLER_TEST_DIR ${CMAKE_CURRENT_BINARY_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/constants.hpp.in
               ${CMAKE_CURRENT_BINARY_DIR}/constants.hpp)

add_executable(unit_tests
               main.cpp
               cmake_tests.cpp
               dependency_tests.cpp
               location_tests.cpp
               object_tests.cpp
               project_tests.cpp
               init_tests.cpp
               version_constraint_tests.cpp 
               version_tests.cpp
)

set_property(TARGET unit_tests PROPERTY CXX_STANDARD 17)
target_include_directories(unit_tests PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(unit_tests antler-project Catch2::Catch2)
catch_discover_tests(unit_tests)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/example.yml DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/project.yml DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#add_subdirectory(fuzz)