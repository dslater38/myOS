#ifndef MULTIBOOTINFOHEADER_H_INCLUDED__
#define MULTIBOOTINFOHEADER_H_INCLUDED__
#include "multiboot2.h"

struct MultiBootInfoHeader
{
	uint32_t		size;
	uint32_t		reserved;
	multiboot_tag	tags[0];
};


#endif // MULTIBOOTINFOHEADER_H_INCLUDED__
