/// @copyright See `LICENSE` in the root directory of this project.

#include <iostream>
#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include <optional>
#include <tuple>

#include "CLI11.hpp"

#include "add_to.hpp"
#include "build.hpp"
#include "init.hpp"
#include "populate.hpp"
#include "remove_from.hpp"
#include "update.hpp"
#include "validate.hpp"

#include <antler/system/version.hpp>


template <typename T, typename V>
static V depends(V&& v) { return std::forward<V>(v); }

template <typename... Ts>
struct runner {
   runner(CLI::App& app)
      : tup(depends<Ts>(app)...), _app(app) {}

   template <std::size_t I=0>
   constexpr inline int exec() {
      if constexpr (I == sizeof...(Ts)) {
         std::cout << _app.help();
         return 0;
      } else {
         if (*std::get<I>(tup).subcommand) {
            return std::get<I>(tup).exec();
         } else {
            return exec<I+1>();
         }
      }

   }
   std::tuple<Ts...> tup;

   private:
      const CLI::App& _app;
};

int main(int argc, char** argv) {
   // using this as we will alias antler-proj to cdt-proj for the
   // next release of CDT.
   const auto app_name = antler::system::fs::path(argv[0]).filename().string();

   CLI::App app{"Antelope Smart Contract Project Management Tool", app_name};

   // Add version flag with callback here.
   app.add_flag_callback("-V,--version",
         [&app]() {
            std::cout << app.get_name() << " v" << antler::system::version::full() << std::endl;
            std::exit(0);       // Succesfull exit MUST happen here.
         },
         "get the version of antler-proj");


   runner<antler::add_to_project,
          antler::build_project,
          antler::init_project,
          antler::populate_project,
          antler::remove_from_project,
          antler::update_project,
          antler::validate_project> runner{app};

   CLI11_PARSE(app, argc, argv);

   try {
      return runner.exec();
   } catch(const std::exception& ex) {
      antler::system::error_log("{}", ex.what());
      return -1;
   } catch(...) {
      antler::system::error_log("unhandled exception");
      return -2;
   }
}
