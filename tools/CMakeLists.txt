# @copyright See `LICENSE` in the root directory of this project.

include(FindPkgConfig)

pkg_check_modules(CURL libcurl REQUIRED)

add_executable(antler-proj main.cpp)
set_property(TARGET antler-proj PROPERTY CXX_STANDARD 17)
target_link_libraries(antler-proj PUBLIC antler-project ${CURL_LIBRARIES})
target_include_directories(antler-proj PRIVATE . ${CURL_INCLUDE_DIRS})