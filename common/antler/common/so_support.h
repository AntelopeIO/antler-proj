#ifndef antler_so_support_h
#define antelr_so_support_h


#ifdef __cplusplus
extern "C" {
#endif

// Support for shared libs.


#if defined(_WIN32)
#define SO_IMPORT __declspec(dllimport)
#define SO_EXPORT __declspec(dllexport)
#define SO_LOCAL
#define SO_NEITHER
#elif defined(__GNUC__)
#define SO_IMPORT __attribute__((visibility("default")))
#define SO_EXPORT __attribute__((visibility("default")))
#define SO_LOCAL __attribute__((visibility("hidden")))
#define SO_NEITHER __attribute__((visibility("default")))
#else
#error "Your comiler needs Shared Object defines!"
#endif



#ifdef __cplusplus
} // extern "C"
#endif

#endif
