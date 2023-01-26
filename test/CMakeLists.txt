include(CTest)

# I know. But it's appropriate for now.
file(GLOB src_cpp ./*.cpp)

foreach(file_name IN ITEMS ${src_cpp})
  cmake_path(GET file_name FILENAME temp)
  string(REPLACE ".cpp" "" name ${temp})
  set(name "test-${name}")

  add_executable(${name} ${file_name})
  set_property(TARGET ${target_name} PROPERTY CXX_STANDARD 20)
  target_link_libraries(${name} antler-project)
  target_include_directories(${name} PRIVATE ./)
  add_test(NAME ${name} COMMAND ${name})
endforeach()
