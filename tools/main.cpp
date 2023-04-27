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
   const auto app_name = antler::system::fs::path(argv[0]).filename().string();

   CLI::App app{"Antelope Smart Contract Project Management Tool", app_name};

   // Add version flag with callback here.
   app.add_flag_callback("-V,--version",
         [&app]() {
            std::cout << app.get_name() << " v" << antler::system::version::full() << std::endl;
            std::exit(0);       // Successful exit MUST happen here.
         },
         "get the version of antler-proj");


   runner<antler::add_to_project,
          antler::build_project,
          antler::init_project,
          antler::populate_project,
          antler::remove_from_project,
          antler::update_project,
          antler::validate_project> runner{app};

   try {
      app.parse(argc, argv);
   }
   // This hack fix parsing error handling of an empty command argument. Example: antler-proj build ~/path/proj ""
   catch (const CLI::ExtrasError& e) {

      std::string error_message(e.what());
      CLI::detail::trim(error_message);

      if (error_message == "The following argument was not expected:" ||
          error_message == "The following arguments were not expected:") {
         return app.exit(CLI::ExtrasError("Empty argument(s) encountered in the command line.", e.get_exit_code()));
      }
      return app.exit(e);
   } catch (const CLI::ParseError& e) {
      return app.exit(e);
   }

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
