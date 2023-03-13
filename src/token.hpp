#pragma once

#include <cstdint>
#include <string_view>
#include <vector>
#include <iostream>
#include <unordered_map>

#include <magic_enum.hpp>

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
      compile_options,
      link_options,
      patch,
      project,
      release,
      tag,
      tests,
      version,
      error
   };
} // namespace antler::project
