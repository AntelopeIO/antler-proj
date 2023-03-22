include(FindPkgConfig)

pkg_check_modules(CURL libcurl REQUIRED)

add_library(antler-project
   cmake_templates.cpp
   dependency.cpp 
   location.cpp 
   populator.cpp
   project.cpp
   version_constraint.cpp 
)

target_include_directories(antler-project PUBLIC ../include 
                                                 ${CMAKE_CURRENT_BINARY_DIR}/../include
                                                 ${CMAKE_SOURCE_DIR}/external/Mustache
                                                 ${CURL_INCLUDE_DIRS}
)
target_link_libraries(antler-project PUBLIC mustache
                                            nlohmann_json::nlohmann_json 
                                            yaml-cpp::yaml-cpp
                                            bluegrass::cturtle
                                            ${CURL_LIBRARIES}
                                            -lstdc++fs)
set_property(TARGET antler-project PROPERTY CXX_STANDARD 17)