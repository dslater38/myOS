#include "vesavga.h"
#include "vesavga.h"
#include "common.h"


static u16int cursor_x;
static u16int cursor_y;
static u8int backColor = BLACK;
static u8int foreColor = CYAN;

#define COLS 80
#define COLS2 (COLS/2)
#define ROWS 25

#define VIDEO_MEM_COUNT ROWS*COLS

static u16int *video_memory=(u16int *)0xB8000;

static _Alignas(4) u16int back_buffer[VIDEO_MEM_COUNT] = {0};
static u32int	cur_line = 0;
// static u32int	count = 0;

bool set_foreground_color32(u8int clr)
{
	bool success = false;
	if( clr < 16 )
	{
		foreColor = clr;
		success = true;
	}
	return success;
}

bool set_background_color32(u8int clr)
{
	bool success = false;
	if( clr < 16 )
	{
		backColor = clr;
		success = true;
	}
	return success;
}


static void inc_cur_line()
{
	cur_line = (cur_line + 1) % ROWS;
}

static u16int color_attribute()
{
	
	return (u16int)((backColor << 4) | (foreColor & 0x0F));
}

static u16int colored_char(char c)
{
	return ((color_attribute() << 8) | c);
}

static void copy_buffer()
{
	u32int *dst = (u32int *)video_memory;
	u32int *src=(u32int *)back_buffer;
	u32int *end = (u32int *)(back_buffer + ROWS*COLS);
	
	while(src<end)
	{
		*dst ++= *src++;
	}
}


static void copy_line2(int dstLine, int srcLine)
{
	u32int *pDest = (u32int *)(video_memory + COLS*dstLine);
	u32int *pSrc= (u32int *)(back_buffer + COLS*srcLine);
	u32int *pEnd = pSrc + COLS2;
	while(pSrc<pEnd)
	{
		*pDest++ = *pSrc++;
	}
}

static void blank_line(u32int line)
{
	if( line >= 0 && line < ROWS )
	{
//		u8int attributeByte = color_attribute();
//		u16int blank = 0x20 /* space */ | (attributeByte << 8);
		u16int blank = colored_char( (char)0x20 );
		u32int blank32 = (((u32int)blank) << 16)|blank;
		u32int *ptr = (u32int *)(back_buffer + line*COLS);
		u32int *pEnd = ptr + COLS2;
		while(ptr < pEnd)
		{
			*ptr++ = blank32;
		}
	}
}


static void copy_buffer2()
{
	int i;
	int j=0;
	if(cursor_y == (ROWS-1))
	{
		for(i=cur_line+1; i<ROWS; ++i )
		{
			copy_line2(j++,i);
		}
	}
	
	for(i=0; i<=cur_line; ++i )
	{
		copy_line2(j++,i);
	}
}


// Updates the hardware cursor.
static void move_cursor()
{
   // The screen is COLS characters wide...
   u16int cursorLocation = cursor_y * COLS + cursor_x;
   outb(0x3D4, 14);                  // Tell the VGA board we are setting the high cursor byte.
   outb(0x3D5, cursorLocation >> 8); // Send the high cursor byte.
   outb(0x3D4, 15);                  // Tell the VGA board we are setting the low cursor byte.
   outb(0x3D5, cursorLocation);      // Send the low cursor byte.
}

static void copy_line(int dstLine, int srcLine)
{
	u32int *pDest = (u32int *)(video_memory + COLS*dstLine);
	u32int *pEnd = pDest + 12;
	u32int *pSrc= (u32int *)(video_memory + COLS*srcLine);
	for(;pDest<pEnd;++pDest,++pSrc)
	{
		*pDest = *pSrc;
	}
}

// Scrolls the text on the screen up by one line.
static void scroll()
{
	// Row ROWS is the end, this means we need to scroll up
	if(cursor_y >= ROWS)
	{
	   
		// The last line should now be blank.
		blank_line(cur_line);
		// The cursor should now be on the last line.
		cursor_y = (ROWS-1);
	}
}

// Writes a single character out to the screen.
void monitor_put32(char c)
{
	outb(0xe9, c);
	switch(c)
	{
		case 0x08:
			if(cursor_x)
			{
				--cursor_x;
			}
			break;
		case 0x09:
			cursor_x = (cursor_x+8) & ~(8-1);
			break;
		case '\r':
			cursor_x = 0;
			break;
		case '\n':
			cursor_x = 0;
			++cursor_y;
			inc_cur_line();
			break;
		default:
			if(c >= ' ')
			{
				// The background colour is black (0), the foreground is white (15).
				// u8int backColour = 0;
				// u8int foreColour = 15;

				// The attribute byte is made up of two nibbles - the lower being the
				// foreground colour, and the upper the background colour.
				// u8int  attributeByte = color_attribute();
				// The attribute byte is the top 8 bits of the word we have to send to the
				// VGA board.
				// u16int attribute = attributeByte << 8;
				//  u16int *location = (u16int *)(video_memory + (cursor_y*COLS + cursor_x));
				u16int *location = (u16int *)(back_buffer + (cur_line*COLS + cursor_x));
				*location = colored_char(c);
				// *location = (c | attribute);
				++cursor_x;				   
			}
			break;
	}

	// Check if we need to insert a new line because we have reached the end
	// of the screen.
	if (cursor_x >= COLS)
	{
		cursor_x = 0;
		cursor_y ++;
		inc_cur_line();
	}

	// Scroll the screen if needed.
	scroll();
	// copy the back buffer to frone
	copy_buffer2();
	// Move the hardware cursor.
	move_cursor();
}

// Clears the screen, by copying lots of spaces to the framebuffer.
void monitor_clear32()
{
	// u8int attributeByte = color_attribute();
	// u16int blank = 0x20 /* space */ | (attributeByte << 8);
	u16int blank = colored_char((char)0x20);
	u32int blank32 = (((u32int)blank) << 16)|blank;

	for(u32int *pStart = (u32int *)back_buffer, *pEnd=pStart + COLS2*ROWS;
		pStart < pEnd;
		++pStart )
	{
		*pStart = blank32;
	}
	cur_line = 0;
	// Move the hardware cursor back to the start.
	cursor_x = 0;
	cursor_y = 0;
	copy_buffer();
	move_cursor();	
}

// Outputs a null-terminated ASCII string to the monitor.
void monitor_write32(const char *c)
{
	while(*c)
	{
		monitor_put32(*c);
		++c;
	}
}

void monitor_write_hex32(u32int n)
{
	// TODO: implement this yourself!
	char buffer[32];
	sprintf32(buffer, "%x", n);
	monitor_write32(buffer);
}

void monitor_write_dec32(u32int n)
{
	// TODO: implement this yourself!
	char buffer[32];
	sprintf32(buffer, "%d", n);
	monitor_write32(buffer);
}

int puts32(const char *s)
{
	monitor_write32(s);
	return 1;
}