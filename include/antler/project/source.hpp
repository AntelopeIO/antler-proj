#pragma once

#include <fstream>
#include <string_view>

#include "project.hpp"

namespace antler::project {
   namespace detail {
      template <typename Obj>
      inline static std::string_view dir() { 
         if constexpr (std::is_same_v<app_t, Obj>)
            return "apps";
         else if constexpr (std::is_same_v<lib_t, Obj>)
            return "libs";
         else
            throw std::runtime_error("internal failure");
      }
   } // namespace antler::project::detail

   // TODO in the proper release of ANTLER this system will be
   // replaced with a dynamic system the frontend dev (not smart contract dev) would supply
   template <typename Ty>
   struct source;

   template <>
   struct source <app_t> {
      inline static void create_source_file(system::fs::path p, const app_t& obj) {
         std::string name = std::string(obj.name());
         p /= system::fs::path("apps") / name / (name+".cpp");
         std::ofstream src{p.c_str()};
         src << "#include <" << name << ".hpp>\n\n";
         src << "[[eosio::action]]\n";
         src << "void " << name << "::hi( name nm ) {\n";
         src << "   /* generated example action */\n";
         src << "   print_f(\"Hello : %\", nm);\n";
         src << "}\n";
         src << std::endl;
         src.close();
      }

      inline static void create_specification_file(system::fs::path p, const app_t& obj) {
         std::string name = std::string(obj.name());
         p /= system::fs::path("include") / name / (name+".hpp");
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

   template <>
   struct source <lib_t> {
      inline static void create_source_file(system::fs::path p, const lib_t& obj) {
         std::string name = std::string(obj.name());
         std::string ext = system::extension(obj.language());
         p /= system::fs::path("libs") / name / (name+ext);
         std::ofstream src{p.c_str()};
         src << "#include <eosio/print.h>\n\n";
         src << "/// Add your code here for the library\n";
         src << std::endl;
         src.close();
      }

      inline static void create_specification_file(system::fs::path p, const lib_t& obj) {}
   };
}