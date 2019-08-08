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

void init_idt64_table();

class Foobar
{
public:
	Foobar(const char *str)
	: m_str{str}
	{}
	const char *get()const { return m_str; }
private:
	const char *m_str{nullptr};
};

// uint32_t mboot_header=0;

volatile int foo___ = 0;
static void test_page_fault();

void cmain (BootInformation &bootInfo, const MultiBootInfoHeader *addr);


extern Foobar barfoo;

extern void initHeap();

extern "C"
{
	void initTextFrameBuffer();
	extern uint64_t placement_address;
	void __libc_init_array (void);


void kmain64(uint32_t magic, const MultiBootInfoHeader *mboot_header)
{
	// Entry - at this point, we're in 64-bit long mode with a basic
	// page table that identity maps the first 2 MB or RAM
	// install our interrupt handlers
	init_idt64_table();
	
	__libc_init_array();

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
		
	initHeap();
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
	}
	else
	{
		PANIC("mboot_header is NULL\n");
	}
	

	test_page_fault();
	

}

}

	
static void test_page_fault()
{
	monitor_write("Testing Page Fault\n");
	uint32_t *ptr = (uint32_t*)0xA0000000;
	uint32_t do_page_fault = *ptr;
	monitor_write_dec(do_page_fault);
	monitor_write("After page fault! SHOULDN'T GET HERE \n");
}

const char *SSS="THis is the initial string";

Foobar barfoo{
	SSS
};