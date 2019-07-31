#include "common.h"
#include "vesavga.h"

extern "C"
{

void panic(const char *message, const char *file, uint32_t line)
{
	// We encountered a massive problem and have to stop.
	// asm volatile("cli"); // Disable interrupts.
	disable_interrupts();

	set_foreground_color(BLACK);
	set_background_color(RED);

	printf("PANIC(%s) at %s : %d\n",message,file,line);
	halt();
}

void panic_assert(const char *file, uint32_t line, const char *desc)
{
	// An assertion failed, and we have to panic.
	// asm volatile("cli"); // Disable interrupts.
	
	disable_interrupts();

	set_foreground_color(BLACK);
	set_background_color(RED);

	printf("ASSERTION-FAILED(%s) at %s : %d\n", desc, file, line);

	halt();
}


void __assert_fail(const char *__assertion, const char *__file,
		   unsigned int __line, const char *__function)
{
	char buffer[1024];
	sprintf(buffer,"Assertion Failed: %s: FUNCTION: %s",__assertion,__function);
	panic(buffer, __file, __line);
}


}
