find_package( Boost REQUIRED COMPONENTS system)
set(Boost_USE_STATIC_LIBS ON)

add_library(antler-project
   #cmake.cpp
   dependency.cpp 
   location.cpp 
   object.cpp 
   project-parse.cpp 
   project-populate.cpp 
   project-print.cpp 
   project.cpp
   #semver.cpp 
   #version_compare.cpp 
   version_constraint.cpp 
   #version.cpp 
)

target_include_directories(antler-project PUBLIC ../include ../external/json/ ${CMAKE_CURRENT_BINARY_DIR}/../include)
target_link_libraries(antler-project PUBLIC magic_enum ryml Boost::system )
set_property(TARGET antler-project PROPERTY CXX_STANDARD 17)