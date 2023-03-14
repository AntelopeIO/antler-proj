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
         constexpr inline static std::string_view filename = "project.yml";

         manifest() = default;
         manifest(const std::filesystem::path& p) 
            : doc(YAML::LoadFile((p / filename).string())),
              path(p) {}
         
         void set(YAML::Node n) {
            doc = std::move(n);
         }

         template <typename T>
         void set(const T& v) {
            doc = std::forward<T>(v);
         }
         
         project to_project() const {
            return doc.as<project>();
         }

         void write(const std::filesystem::path& p) {
            std::ofstream of{(p / filename).string()};
            of << doc;
            of.close();
         }
         
         void write() {
            std::ofstream of{(*path / filename).string()};
            of << doc;
            of.close();
         }
         
      private:
         YAML::Node doc;
         std::optional<std::filesystem::path> path;
   };

} //namespace antler::project