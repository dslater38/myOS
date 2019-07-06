#ifndef KMALLOC_H_INCLUDED
#define KMALLOC_H_INCLUDED

#include "common.h"

u32int kmalloc(u32int sz);
u32int kmalloc_aligned(u32int sz);
u32int kmalloc_phys(u32int sz, u32int *phys);
u32int kmalloc_aligned_phys(u32int sz, u32int *phys);



#endif // KMALLOC_H_INCLUDED
