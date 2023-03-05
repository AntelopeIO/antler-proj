#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include <filesystem>
#include <iostream>
#include <string_view>

#include <antler/project/project.hpp>

inline bool remove_file(std::string_view fn) { return std::filesystem::remove_all(fn); }

inline bool load_project(std::string_view fn, antler::project::project& proj) {
   auto p = std::filesystem::canonical(std::filesystem::path(fn));
   if (!antler::project::project::update_path(p)) {
      return false;
   }

   auto po = antler::project::project::parse(p);
   
   if (po) {
      proj = *po;
      return true;
   }
   return false;
}