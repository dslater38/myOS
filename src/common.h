// common.h -- Defines typedefs and some global functions.
// From JamesM's kernel development tutorials.

#ifndef COMMON_H
#define COMMON_H

// Some nice typedefs, to standardise sizes across platforms.
// These typedefs are written for 32-bit X86.
typedef unsigned long long u64int;
typedef long long s64int;
typedef unsigned int   u32int;
typedef          int   s32int;
typedef unsigned short u16int;
typedef          short s16int;
typedef unsigned char  u8int;
typedef          char  s8int;

#ifndef __cplusplus

typedef		_Bool	bool;
#define true (_Bool)(1)
#define false (_Bool)(0)

#endif

#ifdef __cplusplus
extern "C" {
#endif

void outb(u16int port, u8int value);
void outw(u16int port, u16int value);
void memcpy(void *dest, const void *src, u32int len);
void memset(void *dest, u8int val, u32int len);

extern void enable_interrupts(void);
extern void disable_interrupts(void);
extern void idle_loop(void);	/* never returns */
extern u32int get_fault_addr(void);

u8int inb(u16int port);
u16int inw(u16int port);
int sprintf(char *buf, const char *fmt, ...);
int printf(const char *fmt, ...);

#define NULL 0


void panic(const char *message, const char *file, u32int line);
void panic_assert(const char *file, u32int line, const char *desc);

#define PANIC(msg) panic(msg, __FILE__, __LINE__);
#define ASSERT(b) ((b) ? (void)0 : panic_assert(__FILE__, __LINE__, #b))

#define PAGE_ALIGN(a) if( (a & 0xFFFFF000) != 0 ) { a = ((a & 0xFFFFF000) + 0x1000); }

#ifdef __cplusplus
}
#endif

#endif

