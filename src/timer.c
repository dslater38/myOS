// timer.c -- Initialises the PIT, and handles clock updates.
// Written for JamesM's kernel development tutorials.

#include "timer.h"
#include "isr.h"
#include "vesavga.h"

u32int tick = 0;

static void timer_callback(registers_t regs)
{
	tick++;
	monitor_write("Tick: ");
	monitor_write_dec(tick);
	monitor_write("\n");
}

static void set_repeat_mode()
{
	// Send the command byte.
	outb(PIT_COMMAND_PORT, PIT_SET_MODE_REPEAT);
}

static void set_frequency(u32int frequency)
{

	// The value we send to the PIT is the value to divide it's input clock
	// (1193180 Hz) by, to get our required frequency. Important to note is
	// that the divisor must be small enough to fit into 16-bits.
	u32int divisor = PIT_CLOCK_FREQ / frequency;
	
	// Divisor has to be sent byte-wise, so split here into upper/lower bytes.
	u8int l = (u8int)(divisor & 0xFF);
	u8int h = (u8int)( (divisor>>8) & 0xFF );

	// Send the frequency divisor.
	outb(PIT_CHANNEL_0, l);
	outb(PIT_CHANNEL_0, h);
}

void init_timer(u32int frequency)
{
	// Firstly, register our timer callback.
	register_interrupt_handler(IRQ0, &timer_callback);

	set_repeat_mode();
	set_frequency(frequency);
}

