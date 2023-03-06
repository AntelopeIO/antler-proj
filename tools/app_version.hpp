#pragma once

#include <iostream>
#include <string>

#include "CLI11.hpp"

#include <antler/system/version.hpp>

#include "common.hpp"

namespace antler {
   struct app_version {
      inline app_version(CLI::App& app) {
         app_name = app.get_name();
         subcommand = app.add_subcommand("version", "get the version of antler-proj");
      }

      int32_t exec() { 
         std::cout << app_name << " v" << antler::system::version::full() << std::endl;
         return 0; 
      }

      std::string app_name;
      CLI::App*   subcommand;
   };
} // namespace antler