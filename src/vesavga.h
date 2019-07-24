// monitor.h -- Defines the interface for monitor.h
// From JamesM's kernel development tutorials.

#ifndef MONITOR_H
#define MONITOR_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

// Write a single character out to the screen.
void monitor_put(char c);

// Clear the screen to all black.
void monitor_clear();

// Output a null-terminated ASCII string to the monitor.
void monitor_write(const char *c);
void monitor_write_hex(uintptr_t n);
void monitor_write_dec(uintptr_t n);

uint8_t set_foreground_color(uint8_t clr);
uint8_t set_background_color(uint8_t clr);


#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LTGRAY 7
#define DKGRAY 8
#define LTBLUE 9
#define LTGREEN 10
#define LTCYAN 11
#define LTRED 12
#define LTMAGENTA 13
#define LTBROWN 14
#define WHITE 15

#ifdef __cplusplus
}
#endif


#endif // MONITOR_H

