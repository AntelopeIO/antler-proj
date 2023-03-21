# @copyright See `LICENSE` in the root directory of this project.

add_executable(antler-proj main.cpp)
set_property(TARGET antler-proj PROPERTY CXX_STANDARD 17)
target_link_libraries(antler-proj PUBLIC antler-project)


macro(add_cli_test name file)
   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tests/${file} ${CMAKE_CURRENT_BINARY_DIR}/tests/${file} COPYONLY)
   add_test(NAME ${name} COMMAND python3 ${file} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests)
endmacro()

if (ANTLER_PROJ_BUILD_TESTS)
   enable_testing()
   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tests/util.py ${CMAKE_CURRENT_BINARY_DIR}/tests/util.py COPYONLY)
   #add_cli_test(add_to_project_tests add_to_tests.py)
endif()