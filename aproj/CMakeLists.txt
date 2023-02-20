# @copyright See `LICENSE` in the root directory of this project.


# Boost
find_package( Boost REQUIRED filesystem CONFIG)

#
# These files are generally built in a common way, so here are some functions to simplify the process.
#

function(add_aproj target_name files)

  set(extra_libs "")
  if(target_name STREQUAL "aproj")
    set(extra_libs "Boost::filesystem" "dl")
  endif()

  add_executable(${target_name} ${files})
  set_property(TARGET ${target_name} PROPERTY CXX_STANDARD 20)
  target_link_libraries(${target_name} PUBLIC antler-project ${extra_libs})
  target_include_directories(${target_name} PRIVATE . )
  target_include_directories(${target_name} SYSTEM PRIVATE ../3p_include )
  install(FILES $<TARGET_FILE:${target_name}> COMPONENT Runtime DESTINATION .)
endfunction()


file(GLOB src_cpp ./*.cpp)

foreach(file_name IN ITEMS ${src_cpp})
  if(${CMAKE_VERSION} VERSION_LESS "3.20")
    get_filename_component(fn ${file_name} NAME)
  else()
    cmake_path(GET file_name FILENAME fn)
  endif()

  string(REPLACE ".cpp" "" name ${fn})
  add_aproj("${name}" "${fn}")
endforeach()
