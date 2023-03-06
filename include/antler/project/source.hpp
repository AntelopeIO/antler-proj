#pragma once

#include <filesystem>
#include <fstream>
#include <string_view>

#include "project.hpp"

namespace antler::project {
   namespace detail {
      template <object::type_t Ty>
      inline static std::string_view dir() { 
         if constexpr (Ty == object::type_t::app)
            return "apps";
         else if constexpr (Ty == object::type_t::lib)
            return "libs";
         else
            throw std::runtime_error("internal failure");
      }
   } // namespace antler::project::detail

   // TODO in the proper release of ANTLER this system will be
   // replaced with a dynamic system the frontend dev (not smart contract dev) would supply
   template <object::type_t Ty>
   struct source;

   template <>
   struct source <object::type_t::app> {
      inline static void create_source_file(std::filesystem::path p, const std::string& name) {
         p /= std::filesystem::path("apps") / name / (name+".cpp");
         std::ofstream src{p.c_str()};
         src << "#include <" << name << ".hpp>\n\n";
         src << "[[eosio::action]]\n";
         src << "void " << name << "::hi( name nm ) {\n";
         src << "   /* generated example action */\n";
         src << "   print_f(\"Hello : %\", nm)\n";
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

   template <>
   struct source <object::type_t::lib> {
      inline static void create_source_file(std::filesystem::path p, const std::string& name) {
         p /= std::filesystem::path("libs") / name / (name+".cpp");
         std::ofstream src{p.c_str()};
         src << "#include <eosio/eosio.hpp>\n\n";
         src << "/// Add your code here for the library\n";
         src << std::endl;
         src.close();
      }

      inline static void create_specification_file(std::filesystem::path p, const std::string& name) {}
   };
}