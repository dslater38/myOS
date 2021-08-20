// timer.c -- Initialises the PIT, and handles clock updates.
// Written for JamesM's kernel development tutorials.

#include "timer.h"
#include "isr.h"
#include "vesavga.h"

uint64_t tick = 0;

static void timer_callback(registers64_t regs)
{
	tick++;
//	printf("tick: %ld\n", tick);
}

static void set_repeat_mode()
{
	// Send the command byte.
	outb(PIT_COMMAND_PORT, PIT_SET_MODE_REPEAT);
}

static void set_terminal_mode()
{
	// Send the command byte.
	outb(PIT_COMMAND_PORT, PIT_SET_MODE_TERMINAL_COUNT);
}

static void set_frequency(uint32_t frequency)
{

	// The value we send to the PIT is the value to divide it's input clock
	// (1193180 Hz) by, to get our required frequency. Important to note is
	// that the divisor must be small enough to fit into 16-bits.
	uint32_t divisor = PIT_CLOCK_FREQ / frequency;
	
	// Divisor has to be sent byte-wise, so split here into upper/lower bytes.
	uint8_t l = LOBYTE(divisor);
	uint8_t h =HIBYTE(divisor);

	// Send the frequency divisor.
	outb(PIT_CHANNEL_0, l);
	outb(PIT_CHANNEL_0, h);
}

void init_timer(uint32_t frequency)
{
	// Firstly, register our timer callback.
	register_interrupt_handler64(IRQ0, &timer_callback);

	// set_repeat_mode();
	set_terminal_mode();
	// set_frequency(frequency);
}

