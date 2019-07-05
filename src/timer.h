#ifndef TIMER_H_INCLUDED__
#define TIMER_H_INCLUDED__

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

void init_timer(u32int frequency);

#define PIT_CLOCK_FREQ 1193180

/* I/O ports */

#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL_0 0x40
#define PIT_CHANNEL_1 0x41
#define PIT_CHANNEL_2 0x42


/* I/O commands */
#define PIT_SET_MODE_REPEAT 0x36


#ifdef __cplusplus
}
#endif

#endif // TIMER_H_INCLUDED__
