#pragma once

#include <cstdint>
#include <limits>

/* clang-format off */
#define ANTLER_PROJ_TOKENS(tok_macro, E) \
    tok_macro(apps, 1, E)                \
    tok_macro(command, 2, E)             \
    tok_macro(depends, 3, E)             \
    tok_macro(from, 4, E)                \
    tok_macro(hash, 5, E)                \
    tok_macro(lang, 6, E)                \
    tok_macro(libraries, 7, E)           \
    tok_macro(name, 8, E)                \
    tok_macro(options, 9, E)             \
    tok_macro(patch, 10, E)              \
    tok_macro(project, 11, E)            \
    tok_macro(release, 12, E)            \
    tok_macro(tag, 13, E)                \
    tok_macro(tests, 14, E)              \
    tok_macro(version, 15, E)            \
    tok_macro(error, std::numeric_limits<uint8_t>::max(), E)                  
/* clang-format on */