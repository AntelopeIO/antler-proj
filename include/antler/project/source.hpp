#pragma once

#include <filesystem>
#include <fstream>
#include <string_view>

#include "project.hpp"

namespace antler::project {
   // TODO in the proper release of ANTLER this system will be
   // replaced with a dynamic system the frontend dev (not smart contract dev) would supply
   struct app_source {
      inline static void create_source_file(std::filesystem::path p, const std::string& name) {
         p /= std::filesystem::path("apps") / name / (name+".cpp");
         std::ofstream src{p.c_str()};
         src << "#include <" << name << ".hpp>\n\n";
         src << "[[eosio::action]]\n";
         src << "void " << name << "::hi( name nm ) {\n";
         src << "   /* generated example action */\n";
         src << "   print_f(\"Hello : %\n\", nm)\n";
         src << "}\n";
         src << std::endl;
         src.close();
      }

      inline static void create_specification_file(std::filesystem::path p, const std::string& name) {
         p /= std::filesystem::path("include") / name / (name+".hpp");
         std::ofstream hdr{p.c_str()};
         hdr << "#include <eosio/eosio.hpp>\n\n";
         hdr << "using namespace eosio;\n\n";
         hdr << "class [[eosio::contract]] " << name << ": public contract {\n";
         hdr << "   public:\n";
         hdr << "      using contract::contract;\n\n";
         hdr << "      [[eosio::action]]\n";
         hdr << "      void hi( name nm );\n";
         hdr << "      using hi_action = action_wrapper<\"hi\"_n, &" << name << "::hi>;\n";
         hdr << "};\n";
         hdr << std::endl;
         hdr.close();
      }
   };
}