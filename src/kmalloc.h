#ifndef KMALLOC_H_INCLUDED
#define KMALLOC_H_INCLUDED

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t kmalloc(uint32_t sz);
uint32_t kmalloc_aligned(uint32_t sz);
uint32_t kmalloc_phys(uint32_t sz, uint32_t *phys);
uint32_t kmalloc_aligned_phys(uint32_t sz, uint32_t *phys);


uint64_t kmalloc64(uint32_t sz);
uint64_t kmalloc64_aligned(uint32_t sz);
uint64_t kmalloc64_phys(uint32_t sz, uint64_t *phys);
uint64_t kmalloc64_aligned_phys(uint32_t sz, uint64_t *phys);

#ifdef __cplusplus
}
#endif


#endif // KMALLOC_H_INCLUDED
