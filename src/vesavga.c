#include "vesavga.h"
#include "common.h"

static u16int cursor_x;
static u16int cursor_y;
static u16int*video_memory=(u16int *)0xB8000;

static _Alignas(4) u16int back_buffer[25*80] = {0};
static u32int	top_line = 0;

static void inc_top_line()
{
	++top_line;
	if(top_line>24)
	{
		top_line = 0;
	}
}

static void copy_buffer()
{
	u32int *dst = (u32int *)video_memory;
	u32int *src=(u32int *)back_buffer;
	u32int *end = (u32int *)(back_buffer + 25*80);
	
	while(src<end)
	{
		*dst ++= *src++;
	}
}

static void copy_buffer2()
{
	u32int *src=(u32int *)(back_buffer);
	u32int *end = (u32int *)(back_buffer + top_line*80);
	u32int *dst = (u32int *)video_memory;
	while(src<end)
	{
		*dst ++= *src++;
	}
	
	if(top_line>0)
	{
		src = (u32int *)(back_buffer+top_line*80);
		end = (u32int *)(back_buffer+25*80);
		while(src<end)
		{
			*dst ++= *src++;
		}
	}
}

extern void memmove(void *dst, void *src, u32int size);



// Updates the hardware cursor.
static void move_cursor()
{
   // The screen is 80 characters wide...
   u16int cursorLocation = cursor_y * 80 + cursor_x;
   outb(0x3D4, 14);                  // Tell the VGA board we are setting the high cursor byte.
   outb(0x3D5, cursorLocation >> 8); // Send the high cursor byte.
   outb(0x3D4, 15);                  // Tell the VGA board we are setting the low cursor byte.
   outb(0x3D5, cursorLocation);      // Send the low cursor byte.
}

static void copy_line(int dstLine, int srcLine)
{
	u32int *pDest = (u32int *)(video_memory + 80*dstLine);
	u32int *pEnd = pDest + 12;
	u32int *pSrc= (u32int *)(video_memory + 80*srcLine);
	for(;pDest<pEnd;++pDest,++pSrc)
	{
		*pDest = *pSrc;
	}
}

// Scrolls the text on the screen up by one line.
static void scroll()
{

   // Get a space character with the default colour attributes.
   u8int attributeByte = (0 /*black*/ << 4) | (15 /*white*/ & 0x0F);
   u16int blank = 0x20 /* space */ | (attributeByte << 8);
   u32int blank32 = (((u32int)blank) << 16)|blank;

   // Row 25 is the end, this means we need to scroll up
   if(cursor_y >= 25)
   {
       // Move the current text chunk that makes up the screen
       // back in the buffer by a line
	   
	   // memmove(video_memory,video_memory+80,12*80);
	   
       // The last line should now be blank. Do this by writing
       // 80 spaces to it.
	inc_top_line();
	   int i;
       for (i = top_line*80; i < (top_line+1)*80; i++)
       {
           // video_memory[i] = blank;
	   back_buffer[i] = blank;
       }
	   
/*	   
       u32int *pSrc;
	u32int *pEnd;
	u32int *pDest;
	for( pDest=(u32int *)video_memory, pSrc=pDest+40, pEnd=pDest+12*80;
		pDest < pEnd;
		++pDest,++pSrc )
	   {
		   *pDest = *pSrc;
	   }

	pEnd += 40;
	for(; pDest < pEnd; ++pDest )
	   {
		   *pDest = blank32;
	   }*/	
/*	
       int i;
       for (i = 0*80; i < 24*80; i++)
       {
           video_memory[i] = video_memory[i+80];
       }

       // The last line should now be blank. Do this by writing
       // 80 spaces to it.
       for (i = 24*80; i < 25*80; i++)
       {
           video_memory[i] = blank;
       }
*/	   
       // The cursor should now be on the last line.
       cursor_y = 24;
   }
}

// Writes a single character out to the screen.
void monitor_put(char c)
{
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
			inc_top_line();
			break;
		default:
			if(c >= ' ')
			{
				   // The background colour is black (0), the foreground is white (15).
				   u8int backColour = 0;
				   u8int foreColour = 15;

				   // The attribute byte is made up of two nibbles - the lower being the
				   // foreground colour, and the upper the background colour.
				   u8int  attributeByte = (backColour << 4) | (foreColour & 0x0F);
				   // The attribute byte is the top 8 bits of the word we have to send to the
				   // VGA board.
				   u16int attribute = attributeByte << 8;
				  //  u16int *location = (u16int *)(video_memory + (cursor_y*80 + cursor_x));
				u16int *location = (u16int *)(back_buffer + (top_line*80 + cursor_x));
				   *location = c | attribute;
				    cursor_x++;				   
			}
			break;
	}
#if 0	
   // Handle a backspace, by moving the cursor back one space
   if (c == 0x08 && cursor_x)
   {
       cursor_x--;
   }

   // Handle a tab by increasing the cursor's X, but only to a point
   // where it is divisible by 8.
   else if (c == 0x09)
   {
       cursor_x = (cursor_x+8) & ~(8-1);
   }

   // Handle carriage return
   else if (c == '\r')
   {
       cursor_x = 0;
   }

   // Handle newline by moving cursor back to left and increasing the row
   else if (c == '\n')
   {
       cursor_x = 0;
       cursor_y++;
   }
   // Handle any other printable character.
   else if(c >= ' ')
   {
       location = video_memory + (cursor_y*80 + cursor_x);
       *location = c | attribute;
       cursor_x++;
   }
#endif // 0

   // Check if we need to insert a new line because we have reached the end
   // of the screen.
   if (cursor_x >= 80)
   {
       cursor_x = 0;
       cursor_y ++;
	   inc_top_line();
   }

   // Scroll the screen if needed.
   scroll();
   // copy the back buffer to frone
    copy_buffer2();
   // Move the hardware cursor.
   move_cursor();
}

// Clears the screen, by copying lots of spaces to the framebuffer.
void monitor_clear()
{
   u8int attributeByte = (0 /*black*/ << 4) | (15 /*white*/ & 0x0F);
   u16int blank = 0x20 /* space */ | (attributeByte << 8);
   u32int blank32 = (((u32int)blank) << 16)|blank;

    for(u32int *pStart = (u32int *)back_buffer, *pEnd=pStart + 40*25;
		pStart < pEnd;
		++pStart )
	{
		*pStart = blank32;
	}
	top_line = 0;
  // Move the hardware cursor back to the start.
   cursor_x = 0;
   cursor_y = 0;
	copy_buffer2();
   move_cursor();
	
	#if 0
   // Make an attribute byte for the default colours
   u8int attributeByte = (0 /*black*/ << 4) | (15 /*white*/ & 0x0F);
   u16int blank = 0x20 /* space */ | (attributeByte << 8);
   u32int blank32 = (((u32int)blank) << 16)|blank;

    for(u32int *pStart = (u32int *)video_memory, *pEnd=pStart + 40*25;
		pStart < pEnd;
		++pStart )
	{
		*pStart = blank32;
	}
	
/*   
	
   int i;
   for (i = 0; i < 80*25; i++)
   {
       video_memory[i] = blank;
   }
*/
   // Move the hardware cursor back to the start.
   cursor_x = 0;
   cursor_y = 0;
   move_cursor();
   #endif // 0
}

// Outputs a null-terminated ASCII string to the monitor.
void monitor_write(char *c)
{
   while(*c)
   {
	   monitor_put(*c);
	   ++c;
   }
/*   
   int i = 0;
   while (c[i])
   {
       monitor_put(c[i++]);
   }
   */
}

void monitor_write_hex(u32int n)
{
   // TODO: implement this yourself!
   char buffer[32];
  sprintf(buffer, "%x", n);
   monitor_write(buffer);
}

void monitor_write_dec(u32int n)
{
   // TODO: implement this yourself!
   char buffer[32];
  sprintf(buffer, "%d", n);
   monitor_write(buffer);
}
