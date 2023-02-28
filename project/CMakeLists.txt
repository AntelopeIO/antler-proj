# @copyright See `LICENSE` in the root directory of this project.

set( target_name antler-project )

# Boost
find_package( Boost REQUIRED )

# I know. But it's appropriate to glob for now.
file(GLOB_RECURSE src_cpp ./src/*.cpp)
file(GLOB_RECURSE src_h ./src/*.h ./src/*.hpp)
file(GLOB         inc_h ./antler/project/*.h ./antler/project/*.hpp ./antler/project/*.ipp)

add_library(${target_name} ${src_cpp} ${src_h} ${inc_h})
set_property(TARGET ${target_name} PROPERTY CXX_STANDARD 20)
target_compile_definitions(${target_name} PRIVATE
  BUILDING_AP_PROJ
)
target_include_directories(${target_name} PRIVATE
  src
)
target_include_directories(${target_name} SYSTEM PRIVATE
  ../external/include
)
target_include_directories(${target_name} PUBLIC
  .
)

# Install commands
install(
  FILES ${inc_h}
  DESTINATION antler/project COMPONENT Development
)
