#pragma once

#include <cstdint>
#include <string_view>
#include <vector>
#include <iostream>
#include <unordered_map>

#include <magic_enum.hpp>

//#include <antler/system/preprocessor.hpp>
//#include "token_def.hpp"

namespace antler::project {
   enum class token : uint8_t {
      apps,
      command,
      depends,
      from,
      hash,
      lang,
      libraries,
      name,
      options,
      patch,
      project,
      release,
      tag,
      tests,
      version,
      error
   };

   //inline static to_string(token t) {

   //}

   //inline static std::ostream& operator<<(std::ostream& os, const token& e) {

   //}

   //inline static std::istream& operator>>(std::istream& is, token& e) {
} // namespace antler::project

using namespace magic_enum::ostream_operators;