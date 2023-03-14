# @copyright See `LICENSE` in the root directory of this project.

add_executable(antler-proj main.cpp)
set_property(TARGET antler-proj PROPERTY CXX_STANDARD 17)
target_link_libraries(antler-proj PUBLIC antler-project)