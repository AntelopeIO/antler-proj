# @copyright See `LICENSE` in the root directory of this project.

file(GLOB src_hpp *.hpp)
add_executable(antler-proj main.cpp ${src_hpp})
set_property(TARGET antler-proj PROPERTY CXX_STANDARD 17)
target_link_libraries(antler-proj PUBLIC antler-project)

macro(add_cli_test name file)
   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tests/${file} ${CMAKE_CURRENT_BINARY_DIR}/tests/${file} @ONLY)
   add_test(NAME ${name} COMMAND python3 ${file} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests)
endmacro()

if (ANTLER_PROJ_BUILD_TESTS)
   enable_testing()
   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tests/util.py ${CMAKE_CURRENT_BINARY_DIR}/tests/util.py COPYONLY)
   # TODO renable when add to test code is completed:
   #add_cli_test(add_to_project_tests add_to_tests.py)
   add_cli_test(init_tests init_tests.py)
   add_cli_test(add_and_update add_and_update.py)
   add_cli_test(populate_and_build populate_and_build.py)
endif()
