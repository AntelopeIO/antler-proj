#pragma once

#include <filesystem>

#include <antler/project/project.hpp>

namespace antler {
   using proj_t = std::optional<antler::project::project>;

   inline proj_t load_project(const std::filesystem::path& path) {
      auto p = std::filesystem::canonical(std::filesystem::path(path));
      if (!antler::project::project::update_path(p)) {
         std::cerr << "path either did not exist or no `project.yaml` file could be found." << std::endl;
         return std::nullopt;
      }
      return antler::project::project::parse(p);
   }
}