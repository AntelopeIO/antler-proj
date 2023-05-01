/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.hpp>
#include <antler/project/cmake.hpp>
#include <antler/project/populator.hpp>

#include <catch2/catch.hpp>

#include <sstream>

#include <constants.hpp> // test_dir
#include "common.hpp"

TEST_CASE("Testing cmake emission") {
   using namespace antler::project;

   project proj = {};

   proj.name("test_proj");
   proj.version(antler::project::version{"v1.0.0"});

   std::stringstream ss;

   cmake cm(proj);

   cm.emit_preamble(ss);

   constexpr std::string_view cmake_preamble_expected = "# Generated with antler-proj, modify at your own risk\n"
                                                        "cmake_minimum_required(VERSION 3.11)\n"
                                                        "project(\"test_proj\" VERSION 1.0.0)\n\n";

   REQUIRE( ss.str() == cmake_preamble_expected );


   app_t app = {"test_app", "C++", "", ""};

   dependency dep = {"test_lib", "https://github.com/larryk85/fast_math"};

   app.upsert_dependency(std::move(dep));

   proj.upsert(std::move(app));

   ss.str("");
   cm.emit_project_stub(ss);

   constexpr std::string_view project_expected = "find_package(cdt)\n\n"
                                                 "add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libs ${CMAKE_CURRENT_BINARY_DIR}/libs)\n"
                                                 "add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../tests ${CMAKE_CURRENT_BINARY_DIR}/tests)\n\n";

   REQUIRE( ss.str() == project_expected );
}