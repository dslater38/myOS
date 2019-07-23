// common.h -- Defines typedefs and some global functions.
// From JamesM's kernel development tutorials.

#ifndef COMMON_H
#define COMMON_H

#define strlen strlen32
#include <string.h>
#undef strlen

#include <stdint.h>
#include "sym6432.h"


#ifndef __cplusplus

typedef		_Bool	bool;
#define true (_Bool)(1)
#define false (_Bool)(0)

#endif

#ifdef __cplusplus
extern "C" {
#endif

void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);
void outd(uint16_t port, uint32_t value);

// void *memcpy(void *dest, const void *src, uint32_t len);
// void *memset(void *dest, int val, uint32_t len);

extern void enable_interrupts(void);
extern void disable_interrupts(void);
extern void SYM6432(idle_loop)(void);	/* never returns */
extern uint32_t get_fault_addr(void);
extern uint64_t get_fault_addr64(void);

uint8_t SYM6432(inb)(uint16_t port);
uint16_t SYM6432(inw)(uint16_t port);

STATIC32 int SYM6432(sprintf)(char *buf, const char *fmt, ...);
STATIC32 int SYM6432(printf)(const char *fmt, ...);
STATIC32 size_t SYM6432(strlen)(const char *str);

#ifndef NULL
#define NULL 0
#endif

#define HIDWORD(a) ((uint32_t)(((a) & 0xFFFFFFFF00000000ull) >> 32))
#define LODWORD(a) ((uint32_t)((a) & 0x00000000FFFFFFFFull))

STATIC32 void SYM6432(panic)(const char *message, const char *file, uint32_t line);
STATIC32 void SYM6432(panic_assert)(const char *file, uint32_t line, const char *desc);

#define PANIC(msg) SYM6432(panic)(msg, __FILE__, __LINE__);
#define ASSERT(b) ((b) ? (void)0 : SYM6432(panic_assert)(__FILE__, __LINE__, #b))

#define PAGE_ALIGN(a) if( (a & 0xFFFFF000) != 0 ) { a = ((a & 0xFFFFF000) + 0x1000); }

#ifdef __cplusplus
}
#endif

#endif

