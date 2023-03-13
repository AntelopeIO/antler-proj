#pragma once

#include <ctype.h>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_set>
#include <vector>

#include <magic_enum.hpp>
#include <bluegrass/cturtle.hpp>
#include <bluegrass/meta.hpp>

namespace antler::system {
   namespace detail {
      template<char C>
      constexpr inline bool _is_delim(char c) { return c == C; }

      template <char C, char... Cs>
      constexpr inline bool is_delim(char c) { 
         if constexpr (sizeof...(Cs))
            return (C == c) || is_delim<Cs...>(c); 
         else
            return C == c;
      }
   }

   template <char... Delims>
   inline static std::vector<std::string> split(std::string_view s) {
      std::vector<std::string> strings;

      std::size_t i=0, p=0;

      for (; i < s.size(); i++) {
         if (detail::is_delim<Delims...>(s[i])) {
            strings.emplace_back(s.data()+p, i-p);
            p = i+1;
         }
      }

      if (p < i)
         strings.emplace_back(s.data()+p, i-p);

      return strings;
   }

   inline static std::string to_upper(std::string s) {
      for (auto& c : s)
         c = toupper(c);
      return s;
   }

   template <typename Enum, typename = std::enable_if_t<std::is_enum_v<Enum>>>
   inline static std::string_view to_string(Enum e) { return magic_enum::enum_name(e); }

   template <typename Enum, typename = std::enable_if_t<std::is_enum_v<Enum>>>
   inline static Enum from_string(std::string_view s) { 
      auto sv = magic_enum::enum_cast<Enum>(s);
      if (sv) {
         return *sv;
      } else {
         return Enum::error;
      }
   }

   inline static int32_t execute(std::string_view prog) {
      FILE* h = popen(prog.data(), "r");
      if (h == nullptr) {
         std::cerr << "internal failure, program " << prog << " not found." << std::endl;
         return -1;
      }

      std::array<char, 64> buff;

      std::size_t n;

      while ((n = fread(buff.data(), 1, buff.size(), h)) > 0) {
         fwrite(buff.data(), 1, n, stdout);
      }
      return WEXITSTATUS(pclose(h));
   }

   inline static std::string extension(std::string_view l) {
      if (l == "CXX")
         return ".cpp";
      else
         return ".c";
   }

   template <typename Ex>
   inline static void print_error(Ex&& ex) {
      std::cerr << "manifest error at pos: " << ex.mark.pos << " line: " << ex.mark.line << ", column: " << ex.mark.column << std::endl;
      std::cerr << "   Message: " << ex.msg << std::endl;
   }

   using bluegrass::cturtle::debug_log;
   using bluegrass::cturtle::info_log;
   using bluegrass::cturtle::warn_log;
   using bluegrass::cturtle::error_log;

} // namespace antler::system

// expose all enum operators
using namespace magic_enum::ostream_operators;