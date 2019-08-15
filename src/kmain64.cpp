#include "common.h"
#include "vesavga.h"
#include "serial.h"
#include "ata.h"
#include "multiboot2.h"
#include "PageDirectory.h"
#include "Frames.h"
#include "TextFrameBuffer.h"
#include "MultiBootInfoHeader.h"
#include "BootInformation.h"
#include "Foobar.h"
#include "vfs.h"
#include "timer.h"

void init_idt64_table();

// uint32_t mboot_header=0;

volatile int foo___ = 0;
static void test_page_fault();

void cmain (BootInformation &bootInfo, const MultiBootInfoHeader *addr);

extern Foobar barfoo;

extern Frames<uint64_t> *initHeap();
void mapMemory(Frames<uint64_t> *frames, const multiboot_tag_mmap *mmap);
// uint64_t RTC_currentTime();
void init_rct_interrupts();
// void init_timer(uint32_t frequency);

extern "C"
{
	void initTextFrameBuffer();
	extern uint64_t placement_address;
	void __libc_init_array (void);
	extern void startup_data_start();
	extern void startup_data_end();
	extern void report_idt_info();

	void kmain64(uint32_t magic, const MultiBootInfoHeader *mboot_header)
	{
		// Entry - at this point, we're in 64-bit long mode with a basic
		// page table that identity maps the first 2 MB or RAM
		// install our interrupt handlers
//		init_idt64_table();
		
//		__libc_init_array();

		/*  Am I booted by a Multiboot-compliant boot loader? */
		if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
		{
			char buffer[40];
			sprintf(buffer,"Invalid magic number: 0x%x\n", (unsigned) magic);
			PANIC (buffer);
		}
		
		
		if(mboot_header && placement_address < reinterpret_cast<uint64_t>(mboot_header))
		{
			// if the mboot header is higher up in memory that placement_address
			// then copy the mboot header down to the bottom of our memory
			// so we can allow placement_address to run across the header's memory.
			auto size = mboot_header->size;
			memcpy(reinterpret_cast<void *>(placement_address), reinterpret_cast<const void *>(mboot_header), size);
			mboot_header = reinterpret_cast<const MultiBootInfoHeader *>(placement_address);
			placement_address += size;
		}
		initTextFrameBuffer();
		set_foreground_color((uint8_t)TextColors::GREEN);
		set_background_color((uint8_t)TextColors::BLACK);	
		
		printf("Hello World from 64-bit long mode!!!!!\n");
		
		auto success = init_serial(2, BAUD_115200, BITS_8, PARITY_NONE, NO_STOP_BITS) ;
		if(success)
		{
			printf("Initialized COM2\n");
		}
		
		debug_out("Startup Data Block: start 0x%016.16lx, end: 0x%016.16lx\n",(uint64_t)&startup_data_start, (uint64_t)&startup_data_end);
		report_idt_info();
		
		auto *frames = initHeap();
		printf("Heap Initialized...\n");
		
		success = init_serial(1, BAUD_115200, BITS_8, PARITY_NONE, NO_STOP_BITS) ;
		if( success == SUCCESS)
		{
			printf("Initialized COM1 port\n");
		}

		printf("COM1: %s,\tCOM2: %s\n",identify_uart(1), identify_uart(2));
		printf("COM3: %s,\tCOM4: %s\n",identify_uart(3), identify_uart(4));
		
		// detect ata disks & controllers.
		detectControllers();

		BootInformation bootInfo{};
		
		// process the mboot header.
		if( mboot_header != 0)
		{
			printf("process mboot_header\n");
			cmain(bootInfo, mboot_header);
			mapMemory(frames, bootInfo.mmap);
		}
		else
		{
			PANIC("mboot_header is NULL\n");
		}
		
		// printf("RTC Current Time:\n");
		// RTC_currentTime();
		init_timer(1);
		init_rct_interrupts();
		asm("sti");
		

		while(true)
		{
			// asm("sti");
			asm("hlt");
		}

		// test_page_fault();
		

	}

}

extern Page4K *getPage(void *vaddr);

void mapMemory(Frames<uint64_t> *frames, const multiboot_tag_mmap *mmap)
{
	const uint8_t *end = reinterpret_cast<const uint8_t *>(mmap) + mmap->size;
	for (auto *entry = mmap->entries;
		 reinterpret_cast<const uint8_t *>(entry) < end;
		 entry = reinterpret_cast<const multiboot_memory_map_t *> (reinterpret_cast<const uint8_t *>(entry) + mmap->entry_size))
	{
		// mark anything that's not available as in use
		// so we don't try to use these addresses.
		if( entry->type != MULTIBOOT_MEMORY_AVAILABLE )
		{
			uint64_t nPages = (entry->len>>12);
			frames->markFrames(entry->addr, nPages);
			auto addr = entry->addr;
			for( auto i=0ull; i<nPages; ++i, addr += 0x1000 )
			{
				auto *page = getPage(reinterpret_cast<void *>(addr));
				if( page )
				{
					page->rw = 0; 	// mark the page as read-only
					page->user = 0;	// only kernel access
				}
			}
		}
	}
}

static void test_page_fault()
{
	printf("Testing Page Fault\n");
	uint32_t *ptr = (uint32_t*)0xA0000000;
	uint32_t do_page_fault = *ptr;
	monitor_write_dec(do_page_fault);
	monitor_write("After page fault! SHOULDN'T GET HERE \n");
}


const char *getTheString()
{

	static const char *SSS="This is the initial string";
	return SSS;

}