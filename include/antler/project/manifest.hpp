#pragma once

/// @copyright See `LICENSE` in the root directory of this project.


#include <fstream>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

#include "project.hpp"

#include <yaml-cpp/yaml.h>

namespace antler::project {

   class manifest {
      public:
         manifest() = default;
         manifest(const std::filesystem::path& p) 
            : doc(YAML::LoadFile(p.string())) {}
         
         void set(YAML::Node n) {
            doc = std::move(n);
         }
         
         project to_project() const {
            return doc.as<project>();
         }

         void write(const std::filesystem::path& p) {
            std::ofstream of{p.string()};
            of << doc;
         }
         
      private:
         YAML::Node doc;
   };

} //namespace antler::project