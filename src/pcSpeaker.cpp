#include "common.h"

volatile int foo = 0;

void beep( u16int freq )
{
	u8int note;
	
	volatile int bx, cx;
	
	outb(0x43, 182);
	outb(0x42, (freq & 0xFF) );
	outb(0x42, ((freq>>8) & 0xFF) );
	
	note = inb(0x61);
	
	note |= 3;
	outb(0x61, note);

	for( bx = 0; bx<25; ++bx )
	{
		for( cx = 0; cx < 65535; ++cx )
		{
			foo = cx + bx;
		}
	}
	note = inb(0x61);
	note &= 0xFC;
	outb(0x61, note);

}