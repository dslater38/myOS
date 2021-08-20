#include <stdint.h>
#include "common.h"

extern "C"
{

	/* These magic symbols are provided by the linker.  */
	extern void (*__preinit_array_start []) (void) __attribute__((weak));
	extern void (*__preinit_array_end []) (void) __attribute__((weak));
	extern void (*__init_array_start []) (void) __attribute__((weak));
	extern void (*__init_array_end []) (void) __attribute__((weak));

	extern void _init (void);

	/* Iterate over all the init routines.  */
	void
	__libc_init_array (void)
	{
	  size_t count;
	  size_t i;

	  count = __preinit_array_end - __preinit_array_start;
	  for (i = 0; i < count; i++)
	    __preinit_array_start[i] ();

	   _init ();

	  count = __init_array_end - __init_array_start;
	  for (i = 0; i < count; i++)
	    __init_array_start[i] ();
	}

	void __cxa_pure_virtual()
	{
		PANIC("Pure Virtual Call...");
	}

	
	int atexit( void(*)(void) )
	{
		return 0;
	}
}
