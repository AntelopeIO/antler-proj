#pragma once

/// @copyright See `LICENSE` in the root directory of this project.


#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <utility>  // std::pair
#include <fstream>

#include "../system/utils.hpp"

#include "yaml-cpp/yaml.h"

namespace antler::project {
struct yaml {
   using node_t = YAML::Node;

   inline static node_t load(const system::fs::path& path) { return YAML::LoadFile(path.string()); }

   inline static void write(system::fs::path path, const node_t& node) {
      std::ofstream out(path);
      out << node;
      out.close();
   }

   template <typename F, std::size_t N>
   inline static bool expect_yaml(const yaml::node_t& n, const char (&key)[N], F&& func) {
      if (n[key]) {
         func(n[key]);
         return true;
      } else {
         system::error_log("expected to find {0} at line: {1}", key, n.Mark().line);
         return false;
      }
   }

   template <typename F, std::size_t N>
   inline static bool expect_yaml_all(const yaml::node_t& n, const char (&key)[N], F&& func) {
      if (n[key]) {
         for (auto& x : n[key]) {
            if (!expect_yaml(x, key, std::forward<F>(func)))
               return false;
         }
      } else {
         system::error_log("expected to find {0} at line: {1}", key, n.Mark().line);
         return false;
      }
      return true;
   }



   template <typename F, std::size_t N>
   inline static bool try_yaml(const yaml::node_t& n, const char (&key)[N], F&& func) {
      if (n[key])
         func(n[key]);
      return true;
   }

   template <typename F, std::size_t N>
   inline static bool try_yaml_all(const yaml::node_t& n, const char (&key)[N], F&& func) {
      if (n[key])
         for (auto& x : n[key])
            func(x);

      return true;
   }
};
}  // namespace antler::project

// TODO in the future use something like meta_refl to simply reflect
// the objects and one overload in manifest
/// Overloads for our datatype conversions

#define ANTLER_YAML_CONVERSIONS(type)                    \
   namespace YAML {                                      \
   template <>                                           \
   struct convert<type> {                                \
      static Node encode(const type& v) {                \
         Node n = v.to_yaml();                           \
         return n;                                       \
      }                                                  \
      static bool decode(const YAML::Node& n, type& p) { \
         try {                                           \
            return p.from_yaml(n);                       \
         } catch (YAML::Exception & e) {                 \
            antler::system::error_log(                   \
               "YAML::Exception: {0}", e.what());        \
            return false;                                \
         }                                               \
      }                                                  \
   };                                                    \
   }

#define ANTLER_EXPECT_YAML(NODE, KEY, SETTER, TYPE) \
   antler::project::yaml::expect_yaml(NODE, KEY, [&](auto& n) { SETTER(n.template as<TYPE>()); })

#define ANTLER_TRY_YAML(NODE, KEY, SETTER, TYPE) \
   antler::project::yaml::try_yaml(NODE, KEY, [&](auto& n) { SETTER(n.template as<TYPE>()); })

#define ANTLER_EXPECT_YAML_ALL(NODE, KEY, SETTER, TYPE) \
   antler::project::yaml::expect_yaml_all(NODE, KEY, [&](auto& n) { SETTER(n.template as<TYPE>()); })

#define ANTLER_TRY_YAML_ALL(NODE, KEY, SETTER, TYPE) \
   antler::project::yaml::try_yaml_all(NODE, KEY, [&](auto& n) { SETTER(n.template as<TYPE>()); })

// do not remove this line