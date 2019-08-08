#include "common.h"
#include "vesavga.h"
#include "serial.h"
#include "ata.h"
#include "multiboot2.h"
#include "PageDirectory.h"
#include "Frames.h"
#include "TextFrameBuffer.h"

void init_idt64_table();
void  identify_uart(uint16_t port);

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

void cmain (unsigned long magic, unsigned long addr);

struct BootInformation
{
	uint32_t		size;
	uint32_t		reserved;
	multiboot_tag	tags[0];
};

extern Foobar barfoo;

extern void initHeap();

extern "C"
{
	void initTextFrameBuffer();
	extern uint64_t placement_address;
	void __libc_init_array (void);


void kmain64(uint32_t magic, uint32_t mboot_header)
{
	__libc_init_array();
	if(mboot_header)
	{
		// copy the mboot header down to the bottom of our memory
		// make sure kmalloc doesn't overwrite the boot information header
		BootInformation *info = reinterpret_cast<BootInformation *>(static_cast<uint64_t>(mboot_header));
		auto size = info->size;

		if(placement_address < mboot_header)
		{
			memcpy(reinterpret_cast<void *>(placement_address), reinterpret_cast<const void *>(mboot_header), size);
			mboot_header = static_cast<uint32_t>(placement_address);
			placement_address += size;
		}
	}
	if( init_serial(2, BAUD_115200, BITS_8, PARITY_NONE, NO_STOP_BITS) )
	{
		printf("Initialized COM2\n");
	}
	initTextFrameBuffer();
	initHeap();
	printf("Heap Initialized...\n");
	
	printf("barfoo: %s\n",barfoo.get());

	set_foreground_color((uint8_t)TextColors::GREEN);
	set_background_color((uint8_t)TextColors::BLACK);
//	monitor_clear();
	printf("x\n");
	printf("Hello World from 64-bit long mode!!!!!\n");
	printf("Init the 64-bit interrupt table\n");
	init_idt64_table();
	printf("64-bit interrupt table is initailzed!!!\n");
	
	printf("COM1: "); identify_uart(1);
	printf("COM2: "); identify_uart(2);
	printf("COM3: "); identify_uart(3);
	printf("COM4: "); identify_uart(4);
	
	auto success = init_serial(1, BAUD_38400, BITS_8, PARITY_NONE, NO_STOP_BITS);

	if( success == SUCCESS)
	{
	 	printf("Initialized COM1 port\n");
	}

	detectControllers();

	if( mboot_header != 0)
	{
		printf("Dump mboot_header\n");
		cmain(MULTIBOOT2_BOOTLOADER_MAGIC, mboot_header);
	}
	else
	{
		printf("mboot_header is NULL\n");
	}
	

	test_page_fault();
	
	// int *badPtr = reinterpret_cast<int *>(0x00F0F0F0F0F0F0F0u);
	
	// foo___ = *(badPtr);
	
	// monitor_write_dec(foo___);
	
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