// main.c -- Defines the C-code kernel entry point, calls initialisation routines.
// Made for JamesM's tutorials

#include "vesavga.h"
#include "common.h"
#include "multiboot.h"
#include "timer.h"
#include "paging.h"
	
extern void beep( u16int freq );
extern void vga43(void);

#include "descriptor_tables.h"


void helloWorld();
volatile int foobar = 0;

extern "C" 
{

int kmain(unsigned long magic, multiboot_info_t *mboot_ptr)
{
	char buffer[128];
	init_descriptor_tables();
	
	set_foreground_color( BLUE );
	set_background_color( LTGRAY );
	monitor_clear();
//	monitor_write("Hello World!!!");
	// Initialise all the ISRs and segmentation
	
	// All our initialisation calls will go in here.
	// helloWorld();
	// vga43();
	// monitor_write("Hello World!!!\n");
	//__asm volatile("int $0x3");
	//__asm volatile("int $0x4");
	//enable_interrupts();
	
    initialise_paging();
    monitor_write("Hello, paging world!\n");	
	
    
	
    u32int *ptr = (u32int*)0xA0000000;
    
    sprintf(buffer,"About to access %08.8x\n", (u32int)ptr);
    monitor_write(buffer);
    
    u32int do_page_fault = *ptr;
	
	monitor_write_dec(do_page_fault);
	
	monitor_write("After page fault!\n");
	
//	init_timer(50);

	return 0;
}

}

void helloWorld()
{
	set_foreground_color( BLUE );
	set_background_color( LTGRAY );
	
	char buffer[32];
	int i,k;
	u8int fg,bg;

	for( k=0; k<5; ++k )
	{
		monitor_clear();
		for(i=0;i<40;++i)
		{
			int j;
					
			sprintf(buffer, "{%d, %d}: %s\n", i, k, "Hello World!");
			for(j=0; j<5000000; ++j )
			{
				foobar = j*i;
			}			
			
			
			monitor_write(buffer);
			//~ switch( i % 3 )
			//~ {
				//~ case 0:
					//~ beep(9121);
					//~ break;
				//~ case 1:
					//~ beep(4560);
					//~ break;
				//~ case 2:
					//~ beep(1140);
					//~ break;
			//~ }
		}
	}	
}