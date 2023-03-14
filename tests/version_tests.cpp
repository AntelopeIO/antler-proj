/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/version.hpp>

#include <catch2/catch.hpp>

TEST_CASE("Testing version class") {
   using namespace antler::project;

   {
      version v = {};
      REQUIRE( v.major() == 0 );
      REQUIRE( v.minor() == 0 );
      REQUIRE( v.patch() == 0 );
      REQUIRE( v.tweak() == "");
      REQUIRE( v.to_string() == "0.0.0");
   }

   {
      version v = {1};
      REQUIRE( v.major() == 1 );
      REQUIRE( v.minor() == 0 );
      REQUIRE( v.patch() == 0 );
      REQUIRE( v.tweak() == "");
      REQUIRE( v.to_string() == "1.0.0");
   }

   {
      version v = {1, 1};
      REQUIRE( v.major() == 1 );
      REQUIRE( v.minor() == 1 );
      REQUIRE( v.patch() == 0 );
      REQUIRE( v.tweak() == "");
      REQUIRE( v.to_string() == "1.1.0");
   }

   {
      version v = {1, 1, 1};
      REQUIRE( v.major() == 1 );
      REQUIRE( v.minor() == 1 );
      REQUIRE( v.patch() == 1 );
      REQUIRE( v.tweak() == "");
      REQUIRE( v.to_string() == "1.1.1");
   }

   {
      version v = {1, 1, 1, "rc3"};
      REQUIRE( v.major() == 1 );
      REQUIRE( v.minor() == 1 );
      REQUIRE( v.patch() == 1 );
      REQUIRE( v.tweak() == "rc3");
      REQUIRE( v.to_string() == "1.1.1-rc3");
   }

   {
      uint16_t mv = std::numeric_limits<uint16_t>::max();
      version v = {mv, mv, mv, "rc3"};
      REQUIRE( v.major() == mv );
      REQUIRE( v.minor() == mv );
      REQUIRE( v.patch() == mv );
      REQUIRE( v.tweak() == "rc3");
      REQUIRE( v.to_string() == "65535.65535.65535-rc3");
      REQUIRE( !v.empty() );

      v.clear();
      REQUIRE( v.major() == 0 );
      REQUIRE( v.minor() == 0 );
      REQUIRE( v.patch() == 0 );
      REQUIRE( v.tweak() == "");
      REQUIRE( v.to_string() == "0.0.0");
      REQUIRE( v.empty() );
   }

   {
      version v = {1, 1, 1, "rc3"};
      version v2 = v;
      REQUIRE( v2.major() == 1 );
      REQUIRE( v2.minor() == 1 );
      REQUIRE( v2.patch() == 1 );
      REQUIRE( v2.tweak() == "rc3");
      REQUIRE( v2.to_string() == "1.1.1-rc3");
   }

   {
      version v = {"1.2.3"};
      REQUIRE( v.major() == 1 );
      REQUIRE( v.minor() == 2 );
      REQUIRE( v.patch() == 3 );
      REQUIRE( v.tweak() == "");
      REQUIRE( v.to_string() == "1.2.3");
   }

   {
      version v = {"v1.2.3"};
      REQUIRE( v.major() == 1 );
      REQUIRE( v.minor() == 2 );
      REQUIRE( v.patch() == 3 );
      REQUIRE( v.tweak() == "");
      REQUIRE( v.to_string() == "1.2.3");
   }

   {
      version v = {"V1.2.3"};
      REQUIRE( v.major() == 1 );
      REQUIRE( v.minor() == 2 );
      REQUIRE( v.patch() == 3 );
      REQUIRE( v.tweak() == "");
      REQUIRE( v.to_string() == "1.2.3");
   }

   {
      version v = {"1.1.1-rc3"};
      REQUIRE( v.major() == 1 );
      REQUIRE( v.minor() == 1 );
      REQUIRE( v.patch() == 1 );
      REQUIRE( v.tweak() == "rc3");
      REQUIRE( v.to_string() == "1.1.1-rc3");
   }

   {
      version v = {"v1.1.1-rc3"};
      REQUIRE( v.major() == 1 );
      REQUIRE( v.minor() == 1 );
      REQUIRE( v.patch() == 1 );
      REQUIRE( v.tweak() == "rc3");
      REQUIRE( v.to_string() == "1.1.1-rc3");
   }

   {
      version v = {"V1.1.1-rc3"};
      REQUIRE( v.major() == 1 );
      REQUIRE( v.minor() == 1 );
      REQUIRE( v.patch() == 1 );
      REQUIRE( v.tweak() == "rc3");
      REQUIRE( v.to_string() == "1.1.1-rc3");
   }
}

TEST_CASE("Testing version class comparisons") {
   using namespace antler::project;

   {
      version v1 = {};
      version v2 = {1};
      REQUIRE( v1 < v2);
      REQUIRE( v1 <= v2);

      v1 = {1,0};
      REQUIRE( v1 >= v2 );
      REQUIRE( v1 <= v2 );
      REQUIRE( v1 == v2 );

      v1 = {1, 1};
      REQUIRE( v1 > v2 );
      REQUIRE( v1 >= v2 );

      v2 = {1, 2};
      REQUIRE( v1 < v2 );

      v1 = {1, 0, 1};
      v2 = {1, 2};
      REQUIRE( v1 < v2 );

      v1 = {1, 2, 0, "rc3"};
      v2 = {1, 2, 0, "rc1"};
      REQUIRE( v1 == v2 );

   }
}

TEST_CASE("Testing version yaml conversions") {
   using namespace antler::project;

   version v1 = {"v1.2.3"};
   version v2 = {"2.3.4"};

   yaml::node_t n1 = v1.to_yaml();
   yaml::node_t n2 = v2.to_yaml();

   yaml::node_t n3{std::string("v1.2.3")};
   yaml::node_t n4{std::string("2.3.4")};

   version v3;
   (void)v3.from_yaml(n3);
   version v4;
   (void)v4.from_yaml(n4);

   CHECK( v1.to_string() == v3.to_string() );
   CHECK( v2.to_string() == v4.to_string() );

   version v5;
   version v6;
   (void)v5.from_yaml(n1);
   (void)v6.from_yaml(n2);

   CHECK( v5.to_string() == v1.to_string() );
   CHECK( v6.to_string() == v2.to_string() );
}