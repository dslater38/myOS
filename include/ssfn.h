#include <cstdint>
#include "kmalloc.h"
#include "memcmp.h"

// #define SSFN_IMPLEMENTATION 1
// #define SSFN_CONSOLEBITMAP_HICOLOR 1
#ifndef NULL
#define NULL nullptr
#define defined_NULL
#endif

#define register

#define SSFN_free kfree
#define SSFN_realloc krealloc

#include "ssfn/ssfn.h"

#ifdef defined_NULL
#undef NULL
#endif

#undef register

