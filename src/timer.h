#ifndef TIMER_H_INCLUDED__
#define TIMER_H_INCLUDED__

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

void init_timer(uint32_t frequency);

constexpr uint32_t PIT_CLOCK_FREQ = 1193180;

/* I/O ports */

constexpr uint16_t PIT_COMMAND_PORT = 0x0043;
constexpr uint16_t PIT_CHANNEL_0 = 0x0040;
constexpr uint16_t PIT_CHANNEL_1 = 0x0041;
constexpr uint16_t PIT_CHANNEL_2 = 0x0042;


/* I/O commands */
constexpr uint8_t PIT_SET_MODE_REPEAT = 0x36;
constexpr uint8_t PIT_SET_MODE_TERMINAL_COUNT = 0x30;


#ifdef __cplusplus
}
#endif

#endif // TIMER_H_INCLUDED__
