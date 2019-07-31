#ifndef KMALLOC_H_INCLUDED
#define KMALLOC_H_INCLUDED

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

void *kmalloc(size_t sz);
void *kmalloc_aligned(size_t sz);
void *kmalloc_phys(size_t sz, void **phys);
void *kmalloc_aligned_phys(size_t sz, void **phys);


#ifdef __cplusplus
}
#endif


#endif // KMALLOC_H_INCLUDED
