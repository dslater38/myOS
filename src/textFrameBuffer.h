#ifndef TEXTFRAMEBUFFER_H_INCLUDED
#define TEXTFRAMEBUFFER_H_INCLUDED

#include "types.h"


struct FrameBuffer
{
	uint16_t *video_memory;
	
	void (*put)(char c);
	void (*write)(char *c);
	void (*clear)(void);
	void (*scroll)(void);
	void (*cursor)(void);
};

#endif // TEXTFRAMEBUFFER_H_INCLUDED
