#include "common.h"
#include "vesavga.h"

extern "C"
{

STATIC32 void SYM6432(panic)(const char *message, const char *file, uint32_t line)
{
	// We encountered a massive problem and have to stop.
	// asm volatile("cli"); // Disable interrupts.
	disable_interrupts();

#ifdef __x86_64__
	
	SYM6432(set_foreground_color)(BLACK);
	SYM6432(set_background_color)(RED);

	SYM6432(printf)("PANIC(%s) at %s : %d\n",message,file,line);
#endif
	// monitor_write("PANIC(");
	// monitor_write(message);
	// monitor_write(") at ");
	// monitor_write(file);
	// monitor_write(":");
	// monitor_write_dec(line);
	// monitor_write("\n");
	// Halt by going into an infinite loop.
	SYM6432(idle_loop)();
}

STATIC32 void SYM6432(panic_assert)(const char *file, uint32_t line, const char *desc)
{
	// An assertion failed, and we have to panic.
	// asm volatile("cli"); // Disable interrupts.
	
	disable_interrupts();
#ifdef __x86_64__
	
    SYM6432(set_foreground_color)(BLACK);
    SYM6432(set_background_color)(RED);

	SYM6432(printf)("ASSERTION-FAILED(%s) at %s : %d\n", desc, file, line);
#endif
	
	// monitor_write("ASSERTION-FAILED(");
	// monitor_write(desc);
	// monitor_write(") at ");
	// monitor_write(file);
	// monitor_write(":");
	// monitor_write_dec(line);
	// monitor_write("\n");
	// Halt by going into an infinite loop.
	SYM6432(idle_loop)();
}

}