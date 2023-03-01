/// @copyright See `LICENSE` in the root directory of this project.

#include <iostream>
#include <string>
#include <string_view>
#include <filesystem>
#include <memory>
#include <vector>
#include <optional>
#include <tuple>

#include <CLI11.hpp>

#include <boost/algorithm/string.hpp>        // boost::split()
#include <boost/dll/runtime_symbol_info.hpp> // boost::dll::program_location()

#include <antler/system/exec.hpp>

#include "add_to.hpp"
#include "init.hpp"
#include "populate.hpp"
#include "remove_from.hpp"

template <typename T, typename V>
static V depends(V&& v) { return std::forward<V>(v); }

template <typename... Ts>
struct runner {
   runner(CLI::App& app) 
      : tup(depends<Ts>(app)...) {}

   template <std::size_t I=0>
   constexpr inline int exec() {
      if constexpr (I == sizeof...(Ts)) {
         std::cerr << "Internal error, runner failure" << std::endl;
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
   const auto app_name = std::filesystem::path(argv[0]).filename().string();
   CLI::App app{app_name};

   runner<antler::add_to_project, 
          antler::init_project,
          antler::populate_project,
          antler::remove_from_project> runner{app};

   CLI11_PARSE(app, argc, argv);

   return runner.exec();
}