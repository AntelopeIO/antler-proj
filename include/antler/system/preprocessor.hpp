#pragma once

#include <cstdint>
#include <string_view>
#include <vector>
#include <unordered_map>

// TODO future release use meta_refl for object reflection in general
// additional support for enums will be added

/* clang-format off */
#define _ANTLER_PROJ_CREATE_ENUM(N, A, E) N,
#define _ANTLER_PROJ_CREATE_STRINGS(N, A, E) #N,
#define _ANTLER_PROJ_CREATE_MAP(N, A, E) {#N, E::N},

#define ANTLER_PROJ_CREATE_ENUM(NAME)                       \
   enum class NAME : uint64_t {                             \
      ANTLER_PROJ_## NAME( _ANTLER_PROJ_CREATE_ENUM, NAME) \
   };

#define ANTLER_PROJ_CREATE_STRINGS(NAME)                     \
   static std::vector<std::string_view> NAME ## _strings() { \
      static std::vector<std::string_view> v = {             \
         ANTLER_PROJ_ ## NAME( _ANTLER_PROJ_CREATE_STRINGS,  \
         NAME ) };                                           \
      return v;                                              \
   }

#define ANTLER_PROJ_CREATE_MAP(NAME)                                  \
   static std::unordered_map<std::string_view, NAME> NAME ## _map() { \
      static std::unordered_map<std::string_view, NAME> m = {         \
         ANTLER_PROJ_ ## NAME( _ANTLER_PROJ_CREATE_MAP,               \
         NAME ) };                                                    \
      return m;                                                       \
   }

#define ANTLER_PROJ_CREATE_TO_STR(NAME)                        \
   inline static std::string_view to_string(NAME o) noexcept { \
      const auto& strings = NAME ## _strings();                \
      if (static_cast<uint64_t>(o) >= strings.size())          \
         return "error";                                       \
      return strings[static_cast<uint64_t>(o)];                \
   }

#define ANTLER_PROJ_CREATE_FROM_STR(NAME)                        \
   template <NAME>                                               \
   inline static NAME from_string(std::string_view s) noexcept { \
      return NAME ## _map()[s];                                  \
   }

#define ANTLER_PROJ_CREATE_OPERATOR(NAME)                             \
   inline static std::ostream& operator<<(std::ostream& os, NAME o) { \
      os << to_string(o);                                             \
      return os;                                                      \
   }

#define ANTLER_PROJ_ENUM(NAME)       \
   ANTLER_PROJ_CREATE_ENUM(NAME)     \
   ANTLER_PROJ_CREATE_STRINGS(NAME)  \
   ANTLER_PROJ_CREATE_MAP(NAME)      \
   ANTLER_PROJ_CREATE_TO_STR(NAME)   \
   ANTLER_PROJ_CREATE_FROM_STR(NAME) \
   ANTLER_PROJ_CREATE_OPERATOR(NAME)

/* clang-format on */