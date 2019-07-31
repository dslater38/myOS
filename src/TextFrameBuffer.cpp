#include "TextFrameBuffer.h"
#include "common.h"
#include "serial.h"

uint16_t operator<<(TextColors c, uint16_t n)
{
	return (static_cast<uint16_t>(c) << n);
}

uint16_t operator&(TextColors c, uint16_t n)
{
	return (static_cast<uint16_t>(c) & n);
}

TextColors TextFrameBuffer::set_foreground_color(TextColors clr)
{
	auto oldColor = TextColors::ERROR;
	if( clr < TextColors::LAST )
	{
		oldColor = foreColor;
		foreColor = clr;
	}
	return oldColor;
}

TextColors TextFrameBuffer::set_background_color(TextColors clr)
{
	auto oldColor = TextColors::ERROR;
	if( clr < TextColors::LAST )
	{
		oldColor = backColor;
		backColor = clr;
	}
	return oldColor;
}


void TextFrameBuffer::inc_cur_line()
{
	cur_line = ((++cur_line ) % ROWS);
}

uint16_t TextFrameBuffer::color_attribute()
{
	
	return (uint16_t)((backColor << 4) | (foreColor & 0x0F));
}

uint16_t TextFrameBuffer::colored_char(char c)
{
	return ((color_attribute() << 8) | c);
}

void TextFrameBuffer::copy_line(int dstLine, int srcLine)
{
	void *dst = reinterpret_cast<void *>(video_memory + COLS*dstLine);
	const void *src = reinterpret_cast<void *>(back_buffer + COLS*srcLine);
	
	memcpy(dst, src, COLS*sizeof(uint16_t));
}

void TextFrameBuffer::blank_line(uint32_t line)
{
	if( line >= 0 && line < ROWS )
	{
		uint16_t blank = colored_char( (char)0x20 );
		uint32_t blank32 = (((uint32_t)blank) << 16)|blank;
		uint32_t *ptr = (uint32_t *)(back_buffer + line*COLS);
		uint32_t *pEnd = ptr + (COLS/2);
		while(ptr < pEnd)
		{
			*ptr++ = blank32;
		}
	}
}

void TextFrameBuffer::copy_buffer()
{
	int i;
	int j=0;
	if(cursor_y == (ROWS-1))
	{
		for(i=cur_line+1; i<ROWS; ++i )
		{
			copy_line(j++,i);
		}
	}
	
	for(i=0; i<=cur_line; ++i )
	{
		copy_line(j++,i);
	}	
}

void TextFrameBuffer::fast_copy_buffer()
{
	void *dst = (void *)video_memory;
	const void *src = (void *)back_buffer;
	memcpy(dst,src,ROWS*COLS*sizeof(uint16_t));
}

// Updates the hardware cursor.
void TextFrameBuffer::move_cursor()
{
   // The screen is COLS characters wide...
   uint16_t cursorLocation = cursor_y * COLS + cursor_x;
   outb(0x3D4, 14);                  // Tell the VGA board we are setting the high cursor byte.
   outb(0x3D5, cursorLocation >> 8); // Send the high cursor byte.
   outb(0x3D4, 15);                  // Tell the VGA board we are setting the low cursor byte.
   outb(0x3D5, cursorLocation);      // Send the low cursor byte.
}


// Scrolls the text on the screen up by one line.
void TextFrameBuffer::scroll()
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

void TextFrameBuffer::debug_out(char c)
{
	if( test(flags,TextFlags::ENABLE_DEBUG) )
	{
		outb(0xe9, c);
	}
}

void TextFrameBuffer::serial_out(char c)
{
	if( test(flags,TextFlags::ENABLE_SERIAL) )
	{
		serial_putc(1, c);
	}
}

// Writes a single character out to the screen.
void TextFrameBuffer::put(char c)
{
	debug_out(c);
	serial_out(c);
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
	copy_buffer();
	// Move the hardware cursor.
	move_cursor();
}

// Clears the screen, by copying lots of spaces to the framebuffer.
void TextFrameBuffer::clear()
{
	// uint8_t attributeByte = color_attribute();
	// uint16_t blank = 0x20 /* space */ | (attributeByte << 8);
	uint16_t blank = colored_char((char)0x20);
	uint32_t blank32 = (((uint32_t)blank) << 16)|blank;

	for(uint32_t *pStart = (uint32_t *)back_buffer, *pEnd=pStart + (COLS/2)*ROWS;
		pStart < pEnd;
		++pStart )
	{
		*pStart = blank32;
	}
	cur_line = 0;
	// Move the hardware cursor back to the start.
	cursor_x = 0;
	cursor_y = 0;
	fast_copy_buffer();
	move_cursor();	
}

// Outputs a null-terminated ASCII string to the monitor.
void TextFrameBuffer::write(const char *str)
{
	for( char c = *str; c!='\0'; c=*(++str) )
	{
		put(c);
	}
}
