set( target_name antler_common )

# I know. But it's appropriate to glob for now.
file(GLOB_RECURSE src_cpp ./src/*.cpp)
file(GLOB_RECURSE src_h   ./src/*.h ./src/*.hpp)
file(GLOB_RECURSE inc_h   ./antler/common/*.h ./antler/common/*.hpp)

add_library(${target_name} ${src_cpp} ${src_h} ${inc_h})
set_property(TARGET ${target_name} PROPERTY CXX_STANDARD 20)
target_compile_definitions(${target_name} PRIVATE
  BUILDING_${target_name}
)
target_link_libraries(${target_name} PUBLIC
)
target_include_directories(${target_name} PRIVATE
  src
)
target_include_directories(${target_name} PUBLIC
  .
)


# Install commands
install(
  FILES ${inc_h}
  DESTINATION ${target_name} COMPONENT Development
)
