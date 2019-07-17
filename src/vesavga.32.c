#include "vesavga.h"
#include "vesavga.h"
#include "common.h"


static uint16_t cursor_x;
static uint16_t cursor_y;
static uint8_t backColor = BLACK;
static uint8_t foreColor = CYAN;

#define COLS 80
#define COLS2 (COLS/2)
#define ROWS 25

#define VIDEO_MEM_COUNT ROWS*COLS

static uint16_t *video_memory=(uint16_t *)0xB8000;

static _Alignas(4) uint16_t back_buffer[VIDEO_MEM_COUNT] = {0};
static uint32_t	cur_line = 0;
// static uint32_t	count = 0;

bool set_foreground_color32(uint8_t clr)
{
	bool success = false;
	if( clr < 16 )
	{
		foreColor = clr;
		success = true;
	}
	return success;
}

bool set_background_color32(uint8_t clr)
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

static uint16_t color_attribute()
{
	
	return (uint16_t)((backColor << 4) | (foreColor & 0x0F));
}

static uint16_t colored_char(char c)
{
	return ((color_attribute() << 8) | c);
}

static void copy_buffer()
{
	uint32_t *dst = (uint32_t *)video_memory;
	uint32_t *src=(uint32_t *)back_buffer;
	uint32_t *end = (uint32_t *)(back_buffer + ROWS*COLS);
	
	while(src<end)
	{
		*dst ++= *src++;
	}
}


static void copy_line2(int dstLine, int srcLine)
{
	uint32_t *pDest = (uint32_t *)(video_memory + COLS*dstLine);
	uint32_t *pSrc= (uint32_t *)(back_buffer + COLS*srcLine);
	uint32_t *pEnd = pSrc + COLS2;
	while(pSrc<pEnd)
	{
		*pDest++ = *pSrc++;
	}
}

static void blank_line(uint32_t line)
{
	if( line >= 0 && line < ROWS )
	{
//		uint8_t attributeByte = color_attribute();
//		uint16_t blank = 0x20 /* space */ | (attributeByte << 8);
		uint16_t blank = colored_char( (char)0x20 );
		uint32_t blank32 = (((uint32_t)blank) << 16)|blank;
		uint32_t *ptr = (uint32_t *)(back_buffer + line*COLS);
		uint32_t *pEnd = ptr + COLS2;
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
   uint16_t cursorLocation = cursor_y * COLS + cursor_x;
   outb(0x3D4, 14);                  // Tell the VGA board we are setting the high cursor byte.
   outb(0x3D5, cursorLocation >> 8); // Send the high cursor byte.
   outb(0x3D4, 15);                  // Tell the VGA board we are setting the low cursor byte.
   outb(0x3D5, cursorLocation);      // Send the low cursor byte.
}

static void copy_line(int dstLine, int srcLine)
{
	uint32_t *pDest = (uint32_t *)(video_memory + COLS*dstLine);
	uint32_t *pEnd = pDest + 12;
	uint32_t *pSrc= (uint32_t *)(video_memory + COLS*srcLine);
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
				// uint8_t backColour = 0;
				// uint8_t foreColour = 15;

				// The attribute byte is made up of two nibbles - the lower being the
				// foreground colour, and the upper the background colour.
				// uint8_t  attributeByte = color_attribute();
				// The attribute byte is the top 8 bits of the word we have to send to the
				// VGA board.
				// uint16_t attribute = attributeByte << 8;
				//  uint16_t *location = (uint16_t *)(video_memory + (cursor_y*COLS + cursor_x));
				uint16_t *location = (uint16_t *)(back_buffer + (cur_line*COLS + cursor_x));
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
	// uint8_t attributeByte = color_attribute();
	// uint16_t blank = 0x20 /* space */ | (attributeByte << 8);
	uint16_t blank = colored_char((char)0x20);
	uint32_t blank32 = (((uint32_t)blank) << 16)|blank;

	for(uint32_t *pStart = (uint32_t *)back_buffer, *pEnd=pStart + COLS2*ROWS;
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

void monitor_write_hex32(uint32_t n)
{
	// TODO: implement this yourself!
	char buffer[32];
	sprintf32(buffer, "%x", n);
	monitor_write32(buffer);
}

void monitor_write_dec32(uint32_t n)
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