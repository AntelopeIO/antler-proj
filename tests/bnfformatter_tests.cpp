/// @copyright See `LICENSE` in the root directory of this project.

#include "../tools/bnfformatter.hpp"

#include <catch2/catch.hpp>

TEST_CASE("bnfformatter_make_option_opts") {
   using namespace CLI;

   App app;
   std::string var1, var2;
   const auto* opt1 = app.add_option("opt1", var1, "desc1")->required();
   const auto* opt2 = app.add_option("opt2", var2, "desc2")->default_val("0.0.0");;
   
   BnfFormatter bnfFormatter;

   CHECK(bnfFormatter.make_option_opts(opt1) == " <TEXT>");
   CHECK(bnfFormatter.make_option_opts(opt2) == " [TEXT], default: 0.0.0");
}
