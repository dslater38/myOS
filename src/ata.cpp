// main.c -- Defines the C-code kernel entry point, calls initialisation routines.
// Made for JamesM's tutorials
#if 0

#include "vesavga.h"
#include "common.h"
#include <stdint.h>
#include "ata.h"
#include <algorithm>

void write(void *buffer, uint32_t address, uint32_t size);
void read(void *buffer, uint32_t address, uint32_t size);

#define ATA_PRIMARY_IO 0x1F0
#define ATA_SECONDARY_IO 0x170

#define ATA_PRIMARY_DCR_AS 0x3F6
#define ATA_SECONDARY_DCR_AS 0x376

#define ATA_PRIMARY_IRQ 14
#define ATA_SECONDARY_IRQ 15


void detectControllers()
{
	const unsigned char magic = 0xBF;
	unsigned char test = 0;
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
}

static void ide_poll(uint16_t io)
{
	
	for(int i=0; i< 4; i++)
		inb(io + ATA_REG_ALTSTATUS);

retry:;
	uint8_t status = inb(io + ATA_REG_STATUS);
	//mprint("testing for BSY\n");
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
	unsigned char cmd = ((write != 0) ? ATA_CMD_WRITE_PIO : ATA_CMD_READ_PIO);
	outb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, (0xE0 | (uint8_t)((address >> 24 & 0x0F))));
	outb(ATA_PRIMARY_IO + 1, 0x00);
	outb(ATA_PRIMARY_IO + ATA_REG_SECCOUNT0, 0x01);
	unsigned char c = (unsigned char)(address );
	outb(ATA_PRIMARY_IO + ATA_REG_LBA0, c);
	c = (unsigned char)(address >> 8);
	outb(ATA_PRIMARY_IO + ATA_REG_LBA1, c);
	c = (unsigned char)(address >> 16);
	outb(ATA_PRIMARY_IO + ATA_REG_LBA2, c);
	//c = (unsigned char)(  ((address & 0xFF000000) >> 24) & 0x0F);
	// c |= 0xE0|(1<<4);
	outb(ATA_PRIMARY_IO + ATA_REG_COMMAND, cmd);
	
	ide_poll(ATA_PRIMARY_IO);

}

void write(void *buffer, uint32_t address, size_t size)
{
	setup_for_io(address, size, 1);
	unsigned short *buf = (unsigned short *)buffer;
	
	for( int idx=0;idx< size/2; ++idx )
	{
		// unsigned short temp = (buf[idx*2] | ( buf[idx*2+1] << 8));
		outw(ATA_PRIMARY_IO + ATA_REG_DATA, buf[idx]);
	}
}

void read(void *buffer, uint32_t address, size_t size)
{
	// uint16_t sect[256] ={0};
	
	unsigned char *buf = (unsigned char *)buffer;

	for( auto read_size = std::min(size, 512ul); size>0; size -= read_size, address += read_size/512, buf += read_size )
	{
		setup_for_io(address, read_size, 0);
			
		for( int idx = 0; idx<read_size/2; ++idx )
		{
			*((uint16_t *)(buf + 2*idx)) = inw(ATA_PRIMARY_IO + ATA_REG_DATA);
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
