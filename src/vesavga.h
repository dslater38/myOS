// monitor.h -- Defines the interface for monitor.h
// From JamesM's kernel development tutorials.

#ifndef MONITOR_H
#define MONITOR_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

// Write a single character out to the screen.
void SYM6432(monitor_put)(char c);

// Clear the screen to all black.
void SYM6432(monitor_clear)();

// Output a null-terminated ASCII string to the monitor.
void SYM6432(monitor_write)(const char *c);
void SYM6432(monitor_write_hex)(uintptr_t n);
void SYM6432(monitor_write_dec)(uintptr_t n);

bool SYM6432(set_foreground_color)(uint8_t clr);
bool SYM6432(set_background_color)(uint8_t clr);


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

