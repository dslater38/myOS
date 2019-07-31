// common.h -- Defines typedefs and some global functions.
// From JamesM's kernel development tutorials.

#ifndef COMMON_H
#define COMMON_H

#include <string.h>
#include <cstdint>
#include <cstdio>

#ifdef __cplusplus
extern "C" {
#endif

void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);
void outd(uint16_t port, uint32_t value);

extern void enable_interrupts(void);
extern void disable_interrupts(void);
extern void halt(void);	/* never returns */
extern uint32_t get_fault_addr(void);
extern uint64_t get_fault_addr64(void);

uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);

inline 
uint32_t HIDWORD(uint64_t n)
{
	return static_cast<uint32_t>((n & 0xFFFFFFFF00000000ull) >> 32);
}

inline 
uint32_t LODWORD(uint64_t n)
{
	return static_cast<uint32_t>(n & 0x00000000FFFFFFFFull);
}

void panic(const char *message, const char *file, uint32_t line);
void panic_assert(const char *file, uint32_t line, const char *desc);

#define PANIC(msg) panic(msg, __FILE__, __LINE__);
#define ASSERT(b) ((b) ? (void)0 : panic_assert(__FILE__, __LINE__, #b))

#define PAGE_ALIGN(a) if( (a & 0xFFFFF000) != 0 ) { a = ((a & 0xFFFFF000) + 0x1000); }

#ifdef __cplusplus
}
#endif

#endif

