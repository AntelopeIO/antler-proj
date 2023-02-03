#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#ifdef __cplusplus
extern "C" {
#endif

#include <antler/common/so_support.h>

// Support for shared libs.
#if defined(BUILDING_AP_PROJ)
#define AP_PROJ_API SO_EXPORT
#else
#define AP_PROJ_API SO_IMPORT
#endif


#ifdef __cplusplus
} // extern "C"
#endif
