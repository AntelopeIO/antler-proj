#pragma once

#include <cstdint>
#include <limits>

/* clang-format off */
#define ANTLER_PROJ_tokens(macro, E) \
   macro(apps, 0, E)                 \
   macro(command, 1, E)              \
   macro(depends, 2, E)              \
   macro(from, 3, E)                 \
   macro(hash, 4, E)                 \
   macro(lang, 5, E)                 \
   macro(libraries, 6, E)            \
   macro(name, 7, E)                 \
   macro(options, 8, E)              \
   macro(patch, 9, E)                \
   macro(project, 10, E)             \
   macro(release, 11, E)             \
   macro(tag, 12, E)                 \
   macro(tests, 13, E)               \
   macro(version, 14, E)             \
   macro(error, -1, E)                  
/* clang-format on */