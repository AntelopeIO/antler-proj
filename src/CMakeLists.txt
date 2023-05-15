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

if( ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11))
      OR ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)))
   set(FSLIB stdc++fs)
endif()

target_include_directories(antler-project PUBLIC ../include
                                                 ${CMAKE_CURRENT_BINARY_DIR}/../include
                                                 ${CURL_INCLUDE_DIRS}
)
target_link_libraries(antler-project PUBLIC nlohmann_json::nlohmann_json
                                            yaml-cpp::yaml-cpp
                                            bluegrass::cturtle
                                            ${CURL_LIBRARIES}
                                            ${FSLIB}
                                         )
set_property(TARGET antler-project PROPERTY CXX_STANDARD 17)

add_custom_command(
    TARGET antler-project POST_BUILD
    COMMAND git config core.hooksPath .githooks
    COMMENT "Git hook set to directory .githooks"
)
