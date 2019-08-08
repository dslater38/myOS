#include "common.h"
#include "kmalloc.h"
#include "TextFrameBuffer.h"
#include "NewObj.h"

TextFrameBuffer *video=nullptr;

extern "C"
{
	void initTextFrameBuffer()
	{
		video = New<TextFrameBuffer>();
		if(video)
		{
			video->clear();
		}
	}
	
	uint8_t set_foreground_color(uint8_t c)
	{
		return static_cast<uint8_t>(video->set_foreground_color(static_cast<TextColors>(c)));
	}

	uint8_t set_background_color(uint8_t c)
	{
		return static_cast<uint8_t>(video->set_background_color(static_cast<TextColors>(c)));
	}	
	
	int puts(const char *s)
	{
		video->write(s);
		video->put('\n');
		return 1;
	}
	

	// Outputs a null-terminated ASCII string to the monitor.
	void monitor_write(const char *str)
	{
		video->write(str);
	}

	void monitor_write_hex(uintptr_t n)
	{
		// TODO: implement this yourself!
		uint32_t hi = static_cast<uint32_t>((n & 0xFFFFFFFF00000000ull) >> 32);
		uint32_t lo = static_cast<uint32_t>(n & 0x00000000FFFFFFFFull);
		char buffer[64];
		sprintf(buffer, "%x%08.8x", hi, lo);
		video->write(buffer);
	}

	void monitor_write_dec(uintptr_t n)
	{
		// TODO: implement this yourself!
		char buffer[64];
		sprintf(buffer, "%d", static_cast<uint32_t>(n));
		video->write(buffer);
	}
	
	// Clears the screen, by copying lots of spaces to the framebuffer.
	void monitor_clear()
	{
		video->clear();
	}

	void monitor_put(char c)
	{
		video->put(c);
	}

};

