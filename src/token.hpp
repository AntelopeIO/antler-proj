#pragma once

#include <cstdint>
#include <string_view>
#include <vector>
#include <unordered_map>

#include <antler/system/preprocessor.hpp>
#include "token_def.hpp"

namespace antler::project {
   enum class token : uint8_t {
      ANTLER_PROJ_TOKENS( ANTLER_PROJ_CREATE_ENUM, antler::project::token )
   };

   inline static std::vector<std::string_view> token_strings() {
      static std::vector<std::string_view> v = { ANTLER_PROJ_TOKENS( ANTLER_PROJ_CREATE_STRINGS, antler::project::token ) };
      return v;
   }

   inline static std::unordered_map<std::string_view, token> token_map() {
      static std::unordered_map<std::string_view, token> m = { ANTLER_PROJ_TOKENS( ANTLER_PROJ_CREATE_MAP, antler::project::token ) };
      return m;
   }

   inline static std::string_view token_to_str(token tok) noexcept {
      if (tok == token::error)
         return "error";
      return token_strings()[static_cast<uint8_t>(tok)];
   }

   inline static token token_from_str(std::string_view s) noexcept {
      const auto itr = token_map().find(s);
      if (itr == token_map().end()) {
         return token::error;
      } else {
         return itr->second;
      }
   }

   inline static std::ostream& operator<<(std::ostream& os, token tok) {
      os << token_to_str(tok);
      return os;
   }
} // namespace antler::project
