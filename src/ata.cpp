// main.c -- Defines the C-code kernel entry point, calls initialisation routines.
// Made for JamesM's tutorials
#if 0

#include "vesavga.h"
#include "common.h"
#include <stdint.h>
#include "ata.h"
#include "isr.h"
#include <algorithm>

extern "C" {
	void insl(uint16_t port, void *buffer, uint64_t count);
}

void write(void *buffer, uint32_t address, uint32_t size);
void read(void *buffer, uint32_t address, uint32_t size);

#define ATA_PRIMARY_IO 0x1F0
#define ATA_SECONDARY_IO 0x170

#define ATA_PRIMARY_DCR_AS 0x3F6
#define ATA_SECONDARY_DCR_AS 0x376

#define ATA_PRIMARY_IRQ 14
#define ATA_SECONDARY_IRQ 15

// Channels:
#define      ATA_PRIMARY      0x00
#define      ATA_SECONDARY    0x01
 
// Directions:
#define      ATA_DIR_READ      0x00
#define      ATA_DIR_WRITE     0x01

struct IDEChannelRegisters {
   uint16_t base{0};  // I/O Base.
   uint16_t ctrl{0};  // Control Base
   uint16_t bmide{0}; // Bus Master IDE
   uint8_t  nIEN{0};  // nIEN (No Interrupt);
   uint8_t  interrupt{0}; 
} channels[2] = {
	{ATA_PRIMARY_IO, ATA_PRIMARY_DCR_AS, 0x00, 0x00},
	{ATA_SECONDARY_IO, ATA_SECONDARY_DCR_AS, 0x00, 0x00}
};

static void primary_callback(registers64_t regs)
{
	channels[ATA_PRIMARY].interrupt = 1;
}

static void secondary_callback(registers64_t regs)
{
	channels[ATA_SECONDARY].interrupt = 1;
}

static void ide_wait_irq(uint32_t channel) {
   while (!channels[channel].interrupt)
      asm("hlt");
   channels[channel].interrupt = 0;
}

void ide_initialize(uint32_t BAR0, uint32_t BAR1, uint32_t BAR2, uint32_t BAR3, uint32_t BAR4) 
{
 
   int j, k, count = 0;
 
   // 1- Detect I/O Ports which interface IDE Controller:
   channels[ATA_PRIMARY  ].base  = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
   channels[ATA_PRIMARY  ].ctrl  = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
   channels[ATA_SECONDARY].base  = (BAR2 & 0xFFFFFFFC) + 0x170 * (!BAR2);
   channels[ATA_SECONDARY].ctrl  = (BAR3 & 0xFFFFFFFC) + 0x376 * (!BAR3);
   channels[ATA_PRIMARY  ].bmide = (BAR4 & 0xFFFFFFFC) + 0; // Bus Master IDE
   channels[ATA_SECONDARY].bmide = (BAR4 & 0xFFFFFFFC) + 8; // Bus Master IDE
}

static uint16_t select_port(uint8_t channel, uint8_t reg)
{
	return (reg < 0x08) ? channels[channel].base  + reg - 0x00 :
		   (reg < 0x0C) ? channels[channel].base  + reg - 0x06 :
		   (reg < 0x0E) ? channels[channel].ctrl  + reg - 0x0A :
		   (reg < 0x16) ? channels[channel].bmide + reg - 0x0E : 0;
}

void ide_write(uint8_t channel, uint8_t reg, uint8_t data) {
	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);

	const auto port = select_port(channel, reg);
	if(port > 0 )
	{
		outb(port, data);
	}
	
	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}

uint8_t ide_read(uint16_t channel, uint8_t reg) {
	uint8_t result;
	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);

	const auto port = select_port(channel, reg);
	if(port > 0 )
	{
		result = inb(port);
	}

	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
	return result;
}

void ide_read_buffer(uint8_t channel, uint8_t reg, void *buffer,
                     uint32_t quads) 
{
   if (reg > 0x07 && reg < 0x0C)
      ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);

	const auto port = select_port(channel, reg);
	if( port > 0 )
	{
		insl(port, buffer, quads);
	}

	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}

void detectControllers()
{
	const uint8_t magic = 0xBF;
	uint8_t test = 0;
	outb(0x1F3,magic);
	test = inb(0x1F3);
	monitor_write("Primary IDE controller ");
	if( test == magic )
	{
		monitor_write("present.\n");
	}
	else
	{
		monitor_write("absent.\n");
	}
	test = 0;
	outb(0x173,magic);
	test = inb(0x173);
	monitor_write("Secondary IDE controller ");
	if( test == magic )
	{
		monitor_write("present.\n");
	}
	else
	{
		monitor_write("absent.\n");
	}

	ide_initialize(0x1F0, 0x3F6, 0x170, 0x376, 0x000);
	register_interrupt_handler64(IRQ14, &primary_callback);
   // 2- Enable IRQs:
   ide_write(ATA_PRIMARY  , ATA_REG_CONTROL, 0);

}

static void ide_poll(uint16_t io, uint16_t ctl)
{
	ide_wait_irq(ATA_PRIMARY);
	return ;
	for(int i=0; i< 4; i++)
	{
		ide_read(ATA_PRIMARY, ATA_REG_ALTSTATUS);
	}
//	for(int i=0; i< 4; i++)
//		inb(io + ATA_REG_ALTSTATUS);

retry:;
	// uint8_t status = inb(io + ATA_REG_STATUS);
	uint8_t status = ide_read(ATA_PRIMARY, ATA_REG_ALTSTATUS);
	//mprint("testing for BSY\ATA_PRIMARY, 
	if(status & ATA_SR_BSY) goto retry;
	//mprint("BSY cleared\n");
retry2:	status = inb(io + ATA_REG_STATUS);
	if(status & ATA_SR_ERR)
	{
		PANIC("ERR set, device failure!\n");
	}
	//mprint("testing for DRQ\n");
	if(!(status & ATA_SR_DRQ)) goto retry2;
	//mprint("DRQ set, ready to PIO!\n");
	return;
}

static void setup_for_io(uint32_t address, size_t size, int write)
{
	uint8_t cmd = ((write != 0) ? ATA_CMD_WRITE_PIO : ATA_CMD_READ_PIO);
//	outb(ATA_PRIMARY_DCR_AS, 0);
	ide_write(ATA_PRIMARY, ATA_REG_HDDEVSEL, (0xE0 | (uint8_t)((address >> 24 & 0x0F))));
	// outb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, (0xE0 | (uint8_t)((address >> 24 & 0x0F))));
	
	ide_write(ATA_PRIMARY, ATA_REG_FEATURES, 0x00);
	// outb(ATA_PRIMARY_IO + 1, 0x00);
	ide_write(ATA_PRIMARY, ATA_REG_SECCOUNT0, 0x01);
	// outb(ATA_PRIMARY_IO + ATA_REG_SECCOUNT0, 0x01);
	auto c = static_cast<uint8_t>(address & 0x000000FF);
	ide_write(ATA_PRIMARY, ATA_REG_LBA0, c);
	// outb(ATA_PRIMARY_IO + ATA_REG_LBA0, c);
	c = static_cast<uint8_t>((address >> 8) & 0x000000FF);
	ide_write(ATA_PRIMARY, ATA_REG_LBA1, c);
	// outb(ATA_PRIMARY_IO + ATA_REG_LBA1, c);
	c = static_cast<uint8_t>((address >> 16) & 0x000000FF);
	ide_write(ATA_PRIMARY, ATA_REG_LBA2, c);
	// outb(ATA_PRIMARY_IO + ATA_REG_LBA2, c);
	//c = (unsigned char)(  ((address & 0xFF000000) >> 24) & 0x0F);
	// c |= 0xE0|(1<<4);
	ide_write(ATA_PRIMARY, ATA_REG_COMMAND, cmd);
	// outb(ATA_PRIMARY_IO + ATA_REG_COMMAND, cmd);
	
	ide_poll(ATA_PRIMARY_IO, ATA_PRIMARY_DCR_AS);

}

void write(void *buffer, uint32_t address, size_t size)
{
	setup_for_io(address, size, 1);
	uint16_t *buf = reinterpret_cast<uint16_t *>(buffer);
	
	for( auto idx=0; idx < size/2; ++idx )
	{
		// unsigned short temp = (buf[idx*2] | ( buf[idx*2+1] << 8));
		outw(ATA_PRIMARY_IO + ATA_REG_DATA, buf[idx]);
	}
}

void read(void *buffer, uint32_t address, size_t size)
{
	// uint16_t sect[256] ={0};
	
	uint8_t *buf = reinterpret_cast<uint8_t *>(buffer);

	for( auto read_size = std::min(size, 512ul); size>0; size -= read_size, address += read_size/512, buf += read_size )
	{
		setup_for_io(address, read_size, 0);
			
		for( int idx = 0; idx<read_size/2; ++idx )
		{
			*reinterpret_cast<uint16_t *>(buf + 2*idx) = inw(ATA_PRIMARY_IO + ATA_REG_DATA);
			//unsigned short temp = inw(ATA_PRIMARY_IO + ATA_REG_DATA);
			//buf[2*idx] = (unsigned char)(temp & 0x00FF);
			//buf[2*idx+1] = (unsigned char)( (temp & 0xFF00) >> 8);
			// sect[idx] = inw(ATA_PRIMARY_IO + ATA_REG_DATA);
		}
	}
	//~ for( int idx=0;idx< size/2; ++idx )
	//~ {
		//~ // unsigned short temp = inw(ATA_PRIMARY_IO + ATA_REG_DATA);
		//~ // *(uint16_t *)(buf + 2*idx) = sect[idx];
		//~ buf[2*idx] = (unsigned char)(sect[idx] & 0x00FF);
		//~ buf[2*idx+1] = (unsigned char)( (sect[idx] & 0xFF00) >> 8);
	//~ }
}

#endif // 0
