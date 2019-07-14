#ifndef KMALLOC_H_INCLUDED
#define KMALLOC_H_INCLUDED

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

u32int kmalloc(u32int sz);
u32int kmalloc_aligned(u32int sz);
u32int kmalloc_phys(u32int sz, u32int *phys);
u32int kmalloc_aligned_phys(u32int sz, u32int *phys);


u64int kmalloc64(u32int sz);
u64int kmalloc64_aligned(u32int sz);
u64int kmalloc64_phys(u32int sz, u64int *phys);
u64int kmalloc64_aligned_phys(u32int sz, u64int *phys);

#ifdef __cplusplus
}
#endif


#endif // KMALLOC_H_INCLUDED
