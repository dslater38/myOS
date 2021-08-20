// monitor.h -- Defines the interface for monitor.h
// From JamesM's kernel development tutorials.

#ifndef MONITOR_H
#define MONITOR_H

#include "common.h"
#include <cstddef>

#ifdef __cplusplus
extern "C" {
#endif

namespace vga {
    constexpr uint16_t SC_INDEX=0x03c4;    /* VGA sequence controller */
    constexpr uint16_t SC_DATA = 0x03c5;
    constexpr uint16_t PALETTE_INDEX = 0x03c8;    /* VGA digital-to-analog converter */
    constexpr uint16_t PALETTE_DATA = 0x03c9;
    constexpr uint16_t GC_INDEX = 0x03ce;   /* VGA graphics controller */
    constexpr uint16_t GC_DATA = 0x03cf;
    constexpr uint16_t CRTC_INDEX = 0x03d4;    /* VGA CRT controller */
    constexpr uint16_t CRTC_DATA = 0x03d5;
    constexpr uint16_t INPUT_STATUS_1 = 0x03da;
    constexpr uint8_t MAP_MASK = 0x02;      /* Sequence controller registers */
    constexpr uint16_t ALL_PLANES = 0xff02;
    constexpr uint8_t MEMORY_MODE = 0x04;

    constexpr uint16_t LATCHES_ON = 0x0008;    /* Graphics controller registers */
    constexpr uint16_t LATCHES_OFF = 0xff08;

    constexpr uint8_t HIGH_ADDRESS = 0x0C;      /* CRT controller registers */
    constexpr uint8_t LOW_ADDRESS = 0x0D;
    constexpr uint8_t UNDERLINE_LOCATION = 0x14;
    constexpr uint8_t MODE_CONTROL = 0x17;

    constexpr uint8_t DISPLAY_ENABLE = 0x01;      /* VGA input status bits */
    constexpr uint8_t VRETRACE = 0x08;

    uint8_t *const VGABuffer=reinterpret_cast<uint8_t *>(0x00000000000A0000);

    /**************************************************************************
     *  plot_pixel                                                            *
     *    Plots a pixel in unchained mode                                     *
     **************************************************************************/

    inline void plot_pixel(int x,int y,uint8_t color)
    {
        outb(SC_INDEX, MAP_MASK);          /* select plane */
        outb(SC_DATA,  1 << (x&3) );

        VGABuffer[(y<<6)+(y<<4)+(x>>2)]=color;
    }

}

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


// #define BLACK 0
// #define BLUE 1
// #define GREEN 2
// #define CYAN 3
// #define RED 4
// #define MAGENTA 5
// #define BROWN 6
// #define LTGRAY 7
// #define DKGRAY 8
// #define LTBLUE 9
// #define LTGREEN 10
// #define LTCYAN 11
// #define LTRED 12
// #define LTMAGENTA 13
// #define LTBROWN 14
// #define WHITE 15

#ifdef __cplusplus
}
#endif


#endif // MONITOR_H

