// main.c -- Defines the C-code kernel entry point, calls initialisation routines.
// Made for JamesM's tutorials

extern "C"
{

#include "vesavga.h"
#include "common.h"
}
	
	
int main(struct multiboot *mboot_ptr)
{
  // All our initialisation calls will go in here.

   char buffer[32];
	int i;
  

  monitor_clear();
	for(i=0;;++i)
	{
		 sprintf(buffer, "%d: %s\n", i, "Hello World!");
	monitor_write(buffer);
	}
//  monitor_write("Hello World!!!");
  return 0xDEADBABA;
}
