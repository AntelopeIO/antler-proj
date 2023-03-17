/// @copyright See `LICENSE` in the root directory of this project.

#include <iostream>
#include <string>
#include <string_view>
#include <filesystem>
#include <memory>
#include <vector>
#include <optional>
#include <tuple>

#include "CLI11.hpp"

#include <antler/project/location.hpp>
#include <antler/system/utils.hpp>

#include "add_to.hpp"
#include "app_version.hpp"
#include "build.hpp"
#include "init.hpp"
#include "populate.hpp"
#include "remove_from.hpp"
#include "update.hpp"
#include "validate.hpp"

template <typename T, typename V>
static V depends(V&& v) { return std::forward<V>(v); }

template <typename... Ts>
struct runner {
   runner(CLI::App& app) 
      : tup(depends<Ts>(app)...) {}

   template <std::size_t I=0>
   constexpr inline int exec() {
      if constexpr (I == sizeof...(Ts)) {
         std::cerr << "Please run one of the subcommands available. Use --help to see what is available." << std::endl;
         return -1;
      } else {
         if (*std::get<I>(tup).subcommand) {
            return std::get<I>(tup).exec();
         } else {
            return exec<I+1>();
         }
      }

   }
   std::tuple<Ts...> tup;
};

int main(int argc, char** argv) {
   // using this as we will alias antler-proj to cdt-proj for the 
   // next release of CDT.
   const auto app_name = system::fs::path(argv[0]).filename().string();

   CLI::App app{"Antelope Smart Contract Project Management Tool", app_name};

   runner<antler::add_to_project, 
          antler::app_version,
          antler::build_project,
          antler::init_project,
          antler::populate_project,
          antler::remove_from_project,
          antler::update_project,
          antler::validate_project> runner{app};

   CLI11_PARSE(app, argc, argv);

   return runner.exec();
}