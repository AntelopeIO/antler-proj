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
                                                        "cmake_minimum_required(VERSION 3.10)\n"
                                                        "project(\"test_proj\" VERSION 1.0.0)\n\n";

   REQUIRE(ss.str() == cmake_preamble_expected);


   app_t app = {"test_app", "C++", "", ""};

   dependency dep = {"test_lib", "https://github.com/larryk85/fast_math"};

   app.upsert_dependency(std::move(dep));

   proj.upsert(std::move(app));

   ss.str("");
   cm.emit_project_stub(ss);

   constexpr std::string_view project_expected = "find_package(cdt)\n\n"
                                                 "add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libs ${CMAKE_CURRENT_BINARY_DIR}/libs)\n"
                                                 "add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../tests ${CMAKE_CURRENT_BINARY_DIR}/tests)\n\n";

   REQUIRE(ss.str() == project_expected);
}



TEST_CASE("Testing valid cmake versions") {

   using antler::system::parse_cmake_ver;
   auto ver = [](int major, int minor, int patch) { return std::make_tuple(major, minor, patch); };

   REQUIRE(parse_cmake_ver("cmake version 3.10.2") == ver(3, 10, 2));
   REQUIRE(parse_cmake_ver("cmake version 3.26.0-rc5") == ver(3, 26, 0));
   REQUIRE(parse_cmake_ver("cmake version 4.0.0+build_metadata") == ver(4, 0, 0));
   REQUIRE(parse_cmake_ver("cmake version 3.26.0-rc2.5.6") == ver(3, 26, 0));
}


TEST_CASE("Testing invalid cmake versions") {

   using antler::system::parse_cmake_ver;
   const auto bad = std::make_tuple(-1, -1, -1);

   REQUIRE(parse_cmake_ver("cmake version  3.10.2") == bad);
   REQUIRE(parse_cmake_ver("cmake version 3.40") == bad);
   REQUIRE(parse_cmake_ver("cmake version 2.8.4294967295") == bad);
   REQUIRE(parse_cmake_ver("cmake version 4.0-rc1") == bad);
   REQUIRE(parse_cmake_ver("cmake version 4.0+exploratory") == bad);
}
