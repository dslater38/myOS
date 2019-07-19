#ifndef KMALLOC_H_INCLUDED
#define KMALLOC_H_INCLUDED

#include "common.h"
#include "sym6432.h"

#ifdef __cplusplus
extern "C" {
#endif

uintptr_t SYM6432(kmalloc)(size_t sz);
uintptr_t SYM6432(kmalloc_aligned)(size_t sz);
uintptr_t SYM6432(kmalloc_phys)(size_t sz, size_t *phys);
uintptr_t SYM6432(kmalloc_aligned_phys)(size_t sz, size_t *phys);

#ifndef __x86_64__
#define kmalloc(a) kmalloc32(a)
#define kmalloc_aligned(a) kmalloc_aligned32(a)
#define kmalloc_phys(a,b) kmalloc_phys32(a,b)
#define kmalloc_aligned_phys(a,b) kmalloc_aligned_phys32(a,b)
#endif

//~ uint64_t kmalloc64(uint32_t sz);
//~ uint64_t kmalloc64_aligned(uint32_t sz);
//~ uint64_t kmalloc64_phys(uint32_t sz, uint64_t *phys);
//~ uint64_t kmalloc64_aligned_phys(uint32_t sz, uint64_t *phys);

#ifdef __cplusplus
}
#endif


#endif // KMALLOC_H_INCLUDED
