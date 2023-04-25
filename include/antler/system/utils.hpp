#pragma once

#include <cctype>
#include <cctype>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_set>
#include <vector>

#include <bluegrass/cturtle.hpp>

#include "filesystem.hpp"

namespace antler::system {

   using bluegrass::cturtle::debug_log;
   using bluegrass::cturtle::info_log;
   using bluegrass::cturtle::warn_log;
   using bluegrass::cturtle::error_log;
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

   inline static int32_t execute(std::string_view prog, const std::vector<std::string>& args) {
      std::string cmd{prog};
      for (const auto& arg : args) {
         cmd += " " + arg;
      }

      FILE* h = popen(cmd.c_str(), "r");
      if (h == nullptr) {
         system::error_log("internal failure, program {0} not found.", prog);
         return -1;
      }

      constexpr size_t array_size = 64;
      std::array<char, array_size> buff{};

      std::size_t n = 0;

      while ((n = fread(buff.data(), 1, buff.size(), h)) > 0) {
         fwrite(buff.data(), 1, n, stdout);
      }

      return WEXITSTATUS(pclose(h));
   }

   inline static std::optional<std::string> execute_quiet(std::string_view prog, const std::vector<std::string>& args) {
      std::string cmd{prog};
      for (const auto& arg : args) {
         cmd += " " + arg;
      }
      //redirect stderr to stdout in case caller wants to parse the error
      cmd += " 2>&1";

      FILE* h = popen(cmd.c_str(), "r");
      if (h == nullptr) {
         system::error_log("internal failure, program {0} not found.", prog);
         return std::nullopt;
      }

      constexpr size_t array_size = 64;
      std::array<char, array_size> buff{};

      std::size_t n = 0;
      std::string ret_val = "";

      while ((n = fread(buff.data(), 1, buff.size(), h)) > 0) {
         ret_val += std::string(buff.data(), n);
      }

      return WEXITSTATUS(pclose(h)) == 0 ? std::optional<std::string>{ret_val} : std::nullopt;
   }

   inline static std::string extension(std::string_view l) {
      if (l == "CXX")
         return ".cpp";
      return ".c";
   }

   inline static std::string language_spec(std::string_view l) {
      std::string lan = to_upper(std::string(l));
      if (lan == "CPP" || lan == "C++" || lan == "CXX")
         return "CXX";
      else
         return "C";
   }

   template <typename Ex>
   inline static void print_error(Ex&& ex) {
      system::error_log("yaml error at pos: {0} line: {1}, column: {2}", ex.mark.pos, ex.mark.line, ex.mark.column);
      system::error_log("Error message: {0}", ex.msg);
   }

} // namespace antler::system

#define ANTLER_CHECK(PRED, MSG, ...)                             \
   if (!(LIKELY(!!(PRED)))) {                                    \
      system::error_log(MSG, ##__VA_ARGS__ );                    \
      throw std::runtime_error{fmt::format(MSG, ##__VA_ARGS__)}; \
   }

// do not remove line
