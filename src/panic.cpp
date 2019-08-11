#include "common.h"
#include "vesavga.h"
#include "TextFrameBuffer.h"
#include <stdarg.h>

extern "C"
{

void panic(const char *fmt, const char *file, uint32_t line, ...)
{
	// We encountered a massive problem and have to stop.
	// asm volatile("cli"); // Disable interrupts.
	disable_interrupts();

	auto oldF = set_foreground_color((uint8_t)TextColors::BLACK);
	auto oldB = set_background_color((uint8_t)TextColors::RED);
	char buf[128] = { 0 };
	va_list args;
	va_start(args, line);
	vsprintf(buf, fmt, args);
	va_end(args);
	

	printf("PANIC(%s) at %s : %d",buf,file,line);
	if( oldF != (uint8_t)TextColors::ERROR )
	{
		set_foreground_color(oldF);
	}
	if( oldB != (uint8_t)TextColors::ERROR)
	{
		set_background_color(oldB);
	}
	printf("\n");
	halt();
}

void panic_assert(const char *file, uint32_t line, const char *desc)
{
	// An assertion failed, and we have to panic.
	// asm volatile("cli"); // Disable interrupts.
	
	disable_interrupts();

	auto oldF = set_foreground_color((uint8_t)TextColors::BLACK);
	auto oldB = set_background_color((uint8_t)TextColors::RED);

	printf("ASSERTION-FAILED(%s) at %s : %d", desc, file, line);
	if( oldF != (uint8_t)TextColors::ERROR )
	{
		set_foreground_color(oldF);
	}
	if( oldB != (uint8_t)TextColors::ERROR)
	{
		set_background_color(oldB);
	}
	printf("\n");

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
