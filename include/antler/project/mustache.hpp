#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.hpp>

#include <filesystem> // path
#include <stdexcept>
#include <string_view>

// TODO this library is not very good
// I will create a better one that uses fmt in the near future
#include <mustache.hpp>

namespace antler::project {
   using kainjow::mustache::mustache; 

   template <typename T>
   struct mustache_data {
      template <typename Str>
      inline mustache_data(Str&& key, T&& val)
         : internal_data(key, std::forward<T>(val)) {}
      
      operator kainjow::mustache::data() const { return internal_data; }
      operator kainjow::mustache::data()& { return internal_data; }

      kainjow::mustache::data internal_data;
   };

   template <typename... Ts>
   struct mustache_list {
      mustache_list(Ts&&... ts)
         : internal_list(std::forward<Ts>(ts)...) {}

      std::vector<kainjow::mustache::data> internal_list;
   };

   template <typename T>
   struct mustache_list<T> {
      mustache_list()
   };

} // namespace antler::project