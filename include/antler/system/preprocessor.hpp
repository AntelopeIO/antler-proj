#pragma once

/* clang-format off */

#define ANTLER_PROJ_CREATE_ENUM(N, A, E) N,
#define ANTLER_PROJ_CREATE_STRINGS(N, A, E) #N,
#define ANTLER_PROJ_CREATE_MAP(N, A, E) {#N, E::N},

/* clang-format on */