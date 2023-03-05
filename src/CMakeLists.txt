find_package( Boost REQUIRED COMPONENTS system)
set(Boost_USE_STATIC_LIBS ON)

include(FindPkgConfig)

pkg_check_modules(CURL libcurl REQUIRED)

add_library(antler-project
   dependency.cpp 
   location.cpp 
   object.cpp 
   project-parse.cpp 
   project-populate.cpp 
   project-print.cpp 
   project.cpp
   version_constraint.cpp 
)

target_include_directories(antler-project PUBLIC ../include ../external/json/ ${CMAKE_CURRENT_BINARY_DIR}/../include)
target_link_libraries(antler-project PUBLIC magic_enum ryml Boost::system ${CURL_LIBRARIES})
set_property(TARGET antler-project PROPERTY CXX_STANDARD 17)