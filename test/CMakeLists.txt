# @copyright See `LICENSE` in the root directory of this project.

include(CTest)

# I know. But it's appropriate for now.
file(GLOB src_cpp ./*.cpp)

foreach(file_name IN ITEMS ${src_cpp})
  if(${CMAKE_VERSION} VERSION_LESS "3.20")
    get_filename_component(temp ${file_name} NAME)
  else()
    cmake_path(GET file_name FILENAME temp)
  endif()

  string(REPLACE ".cpp" "" name ${temp})
  #set(name "test-${name}")

  add_executable(${name} ${file_name})
  set_property(TARGET ${name} PROPERTY CXX_STANDARD 17)
  target_link_libraries(${name} antler-project)
  target_include_directories(${name} PRIVATE ./)
  add_test(NAME ${name} COMMAND ${name})
endforeach()
