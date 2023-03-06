/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.hpp>
#include <antler/project/cmake.hpp>

#include <catch2/catch.hpp>

#include <sstream>


TEST_CASE("Testing cmake emission") {
   using namespace antler::project;

   project proj = {};

   proj.name("test_proj");
   proj.version({"v1.0.0"});

   std::stringstream ss;

   cmake::emit_preamble(ss, proj);

   constexpr std::string_view cmake_preamble_expected = "# Generated with antler-proj tool, modify at your own risk\n"
                                                        "cmake_minimum_required(VERSION 3.13)\n"
                                                        "project(\"test_proj\" VERSION 1.0.0)\n"
                                                        "add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/apps)\n"
                                                        "add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs)\n"
                                                        "add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)\n\n";

   REQUIRE( ss.str() == cmake_preamble_expected );


   object app = {object::type_t::app, "test_app", "C++", "", ""};
   //inline dependency(std::string_view name, std::string_view loc, std::string_view tag="", 
   //                  std::string_view rel="", std::string_view hash="") {

   dependency dep = {"test_lib", "https://github.com/larryk85/fast_math"};

   app.upsert_dependency(std::move(dep));

   proj.upsert_app(std::move(app));

   ss.str("");
   cmake::emit_project(ss, proj);

   constexpr std::string_view project_expected = "add_executable(test_app test_app.cpp)\n"
                                                 "target_include_directories(test_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ./ )\n"
                                                 "target_link_libraries(test_app PUBLIC test_lib)\n\n";

   REQUIRE( ss.str() == project_expected );
   std::cout << "SS " << ss.str() << "\n";
}