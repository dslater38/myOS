#include "common.h"
#include "vesavga.h"


extern "C"
{
void kmain64()
{
	
	set_foreground_color64(CYAN);
	set_background_color64(RED);
	monitor_clear64();
	monitor_write64("Hello World from 64-bit long mode!!!!!\n");
	
}

}