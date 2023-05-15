/// @copyright See `LICENSE` in the root directory of this project.

#include "../include/antler/project/object.hpp"
// #include <antler/project/object.hpp>

#include <catch2/catch.hpp>


TEST_CASE("Testing object") {
   using namespace antler::project;
   using v = std::vector<std::string>;

   app_t app1{"test", "C++", "-std=c++11", "-fno-lto"};
   app_t app2{"test", "C++", "-g;-M", "-lm"};

   CHECK(app1.name() == app2.name());
   CHECK(app1.language() == app2.language());
   CHECK(app1.compile_options() == v{"-std=c++11"});
   CHECK(app1.link_options() == v{"-fno-lto"});
   CHECK(app2.compile_options() == v{"-g", "-M"});
   CHECK(app2.link_options() == v{"-lm"});

   dependency dep1 = {"https::github.com/larryk85/foo", "dep1"};
   dependency dep2 = {"https::github.com/larryk85/foo", "dep1"};

   CHECK(app1.dependencies().empty());

   CHECK(!app1.dependency_exists("dep1"));
   CHECK(!app1.find_dependency("dep1"));

   CHECK(app1.upsert_dependency(std::move(dep1)) == false);

   CHECK(app1.dependency_exists("dep1"));
   const auto& d = app1.find_dependency("dep1");

   CHECK(d);
   CHECK(d->name() == dep2.name());
   CHECK(d->location() == dep2.location());
   CHECK(d->tag().empty());
   CHECK(d->hash().empty());

   CHECK(app1.dependencies().size() == 1);
   CHECK(app1.upsert_dependency({"larryk85/foo2"}) == false);
   CHECK(app1.dependencies().size() == 2);
   CHECK(app1.remove_dependency("foo2"));

   CHECK(app1.dependencies().size() == 1);
   CHECK(app1.remove_dependency("dep1"));
   CHECK(app1.dependencies().empty());
}

TEST_CASE("Testing default dependency name") {
   using namespace antler::project;
   using v = std::vector<std::string>;

   app_t app1{"test", "C++", "-std=c++11", "-fno-lto"};
   app_t app2{"test", "C++", "-g;-M", "-lm"};

   CHECK(app1.name() == app2.name());
   CHECK(app1.language() == app2.language());
   CHECK(app1.compile_options() == v{"-std=c++11"});
   CHECK(app1.link_options() == v{"-fno-lto"});
   CHECK(app2.compile_options() == v{"-g", "-M"});
   CHECK(app2.link_options() == v{"-lm"});

   dependency dep1 = {"https::github.com/larryk85/foo"};
   dependency dep2 = {"https::github.com/larryk85/foo"};

   CHECK(app1.dependencies().empty());

   CHECK(!app1.dependency_exists("foo"));
   CHECK(!app1.find_dependency("foo"));

   CHECK(app1.upsert_dependency(std::move(dep1)) == false);

   CHECK(app1.dependency_exists("foo"));
   const auto& d = app1.find_dependency("foo");

   CHECK(d);
   CHECK(d->name() == dep2.name());
   CHECK(d->location() == dep2.location());
   CHECK(d->tag().empty());
   CHECK(d->hash().empty());

   CHECK(app1.dependencies().size() == 1);
   CHECK(app1.upsert_dependency(std::move(dep2)) == true);
   CHECK(app1.dependencies().size() == 1);
   CHECK(app1.remove_dependency("foo"));

   CHECK(app1.dependencies().empty());
}

TEST_CASE("Testing object::is_valid_name") {
   using namespace antler::project;

   CHECK(app_t::is_valid_name("hello"));
   CHECK(app_t::is_valid_name("Hello"));
   CHECK(app_t::is_valid_name("HE_llo"));
   CHECK(app_t::is_valid_name("_HE_llo"));
   CHECK(app_t::is_valid_name("_"));
   CHECK(app_t::is_valid_name("h"));
   CHECK(app_t::is_valid_name("h9"));
   CHECK(app_t::is_valid_name("_h_e_l_l_o_99"));
   CHECK(app_t::is_valid_name("_88_foo_bazBAr_"));

   CHECK(!app_t::is_valid_name(""));
   CHECK(!app_t::is_valid_name("9"));
}

TEST_CASE("Testing object node conversions <pass>") {
   using namespace antler::project;

   app_t app1{"test", "C++", "-std=c++11;foo;bar", "-fno-lto;test;baz"};

   app_t app2;

   YAML::Node node;
   node["test"] = app1;

   app2 = node["test"].as<app_t>();

   CHECK(app1.name() == app2.name());
   CHECK(app1.language() == app2.language());

   CHECK(app1.compile_options().size() == app2.compile_options().size());

   for (std::size_t i = 0; i < app1.compile_options().size(); i++) {
      CHECK(app1.compile_options()[i] == app2.compile_options()[i]);
   }

   CHECK(app1.link_options().size() == app2.link_options().size());

   for (std::size_t i = 0; i < app1.link_options().size(); i++) {
      CHECK(app1.link_options()[i] == app2.link_options()[i]);
   }
}

TEST_CASE("Testing object node conversions 2") {
   using namespace antler::project;

   YAML::Node node;

   node["name"] = std::string("hello");
   node["lang"] = std::string("CXX");

   app_t app;


   CHECK(app.from_yaml(node));

   CHECK(app.name() == "hello");
   CHECK(app.language() == "CXX");
   CHECK(app.compile_options().empty());
   CHECK(app.link_options().empty());
   CHECK(app.dependencies().empty());

   node["compile_options"] = std::string{"-std=c++14;-fno-rtti"};
   node["link_options"]    = std::string{"-fno-lto;-lm"};

   dependency dep1 = {"larryk85/foo"};
   dependency dep2 = {"larryk85/foo2"};

   node["depends"] = std::vector<dependency>{dep1, dep2};

   std::cout << "Node: " << node << std::endl;

   CHECK(app.from_yaml(node));

   CHECK(app.compile_options().size() == 2);
   CHECK(app.link_options().size() == 2);

   CHECK(app.compile_options()[0] == "-std=c++14");
   CHECK(app.compile_options()[1] == "-fno-rtti");

   CHECK(app.link_options()[0] == "-fno-lto");
   CHECK(app.link_options()[1] == "-lm");

   CHECK(app.dependencies().size() == 2);

   CHECK(app.dependencies()["foo"].name() == dep1.name());
   CHECK(app.dependencies()["foo"].location() == dep1.location());

   CHECK(app.dependencies()["foo2"].name() == dep2.name());
   CHECK(app.dependencies()["foo2"].location() == dep2.location());
}

TEST_CASE("Testing object node conversions with dependencies") {
   using namespace antler::project;

   app_t app = {"test", "C", "abc;def", ""};

   app.upsert_dependency({"foo", "https://github.com/larryk85/dune", "v13.3"});
   app.upsert_dependency({"bar", "https://github.com/larryk85/fast_math", "blah"});
   app.upsert_dependency({"baz", "https://github.com/antelopeio/leap", "v2.2.2v"});

   YAML::Node node;
   node["test"] = app;

   app_t app2;
   CHECK(app2.from_yaml(node["test"]));

   CHECK(app.name() == app2.name());
   CHECK(app.language() == app2.language());

   CHECK(app.compile_options().size() == app2.compile_options().size());
   for (std::size_t i = 0; i < app.compile_options().size(); i++) {
      CHECK(app.compile_options()[i] == app2.compile_options()[i]);
   }

   CHECK(app.link_options().size() == app2.link_options().size());
   for (std::size_t i = 0; i < app.compile_options().size(); i++) {
      CHECK(app.compile_options()[i] == app2.compile_options()[i]);
   }

   CHECK(app.dependencies().size() == app2.dependencies().size());
   for (const auto& [k, v] : app.dependencies()) {
      CHECK(app2.dependency_exists(k));
      const auto& d = app2.find_dependency(k);
      CHECK(d); // it is not a null optional
      CHECK(v.name() == d->name());
      CHECK(v.location() == d->location());
      CHECK(v.tag() == d->tag());
      CHECK(v.hash() == d->hash());
   }
}