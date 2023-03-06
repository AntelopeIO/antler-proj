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

target_include_directories(antler-project PUBLIC ../include 
                                                 ${CMAKE_CURRENT_BINARY_DIR}/../include
                                                 ${CURL_INCLUDE_DIRS}
)
target_link_libraries(antler-project PUBLIC magic_enum ryml nlohmann_json::nlohmann_json ${CURL_LIBRARIES})
set_property(TARGET antler-project PROPERTY CXX_STANDARD 17)