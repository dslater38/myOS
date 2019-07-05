// common.h -- Defines typedefs and some global functions.
// From JamesM's kernel development tutorials.

#ifndef COMMON_H
#define COMMON_H

// Some nice typedefs, to standardise sizes across platforms.
// These typedefs are written for 32-bit X86.
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
void memcpy(u8int *dest, const u8int *src, u32int len);
void memset(u8int *dest, u8int val, u32int len);

u8int inb(u16int port);
u16int inw(u16int port);
int sprintf(char *buf, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif

