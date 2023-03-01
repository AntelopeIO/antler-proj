project(APROJ_PROJECT_NAME)

find_package(cdt)

add_contract( APROJ_PROJECT_NAME APROJ_PROJECT_NAME APROJ_PROJECT_NAME.cpp )
target_include_directories( APROJ_PROJECT_NAME PUBLIC ${CMAKE_SOURCE_DIR}/../include )
target_ricardian_directory( APROJ_PROJECT_NAME ${CMAKE_SOURCE_DIR}/../ricardian )