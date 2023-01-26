
#
# These files are generally built in a common way, so here are some functions to simplify the process.
#

function(add_aproj target_name files)
  add_executable(${target_name} ${files})
  set_property(TARGET ${target_name} PROPERTY CXX_STANDARD 20)
  target_link_libraries(${target_name} PUBLIC antler-project)
  target_include_directories(${target_name} PRIVATE . )
  install(FILES $<TARGET_FILE:${target_name}> COMPONENT Runtime DESTINATION .)
endfunction()


file(GLOB src_cpp ./*.cpp)

foreach(file_name IN ITEMS ${src_cpp})
  cmake_path(GET file_name FILENAME fn)
  string(REPLACE ".cpp" "" name ${fn})
  add_aproj("${name}" "${fn}")
endforeach()
