#include "multiboot2.h"

typedef unsigned int uint32_t;

typedef unsigned short uint16_t;

extern void edata(void);
extern void start(void);
extern void end(void);

/* #define MBOOT2_HEADER_MAGIC 0xe85250d6 */

// #define ENABLE_FRAME_BUFFER 1

#define alignas(a) _Alignas(a)

struct MultiBoot
{
	alignas(8) struct multiboot_header header;
//	alignas(8) struct multiboot_header_tag_address	address;
//	alignas(8) struct multiboot_header_tag_entry_address entry;
	alignas(8) struct multiboot_header_tag_console_flags console;
	alignas(8) struct multiboot_header_tag_module_align align;
#ifdef ENABLE_FRAME_BUFFER		
	alignas(8) struct multiboot_header_tag_framebuffer	frame;
#endif	
	alignas(8) struct multiboot_header_tag end;
};


extern alignas(8) struct MultiBoot header;


alignas(8) struct MultiBoot header = {
	{ 	/* Header  */
		MULTIBOOT2_HEADER_MAGIC,
		MULTIBOOT_ARCHITECTURE_I386,
		sizeof(header),
		0x100000000 - (MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + (sizeof(header)))
	} ,
	{
		MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS,
		MULTIBOOT_HEADER_TAG_OPTIONAL,
		sizeof(header.console),
		MULTIBOOT_CONSOLE_FLAGS_EGA_TEXT_SUPPORTED
	},
	{
		MULTIBOOT_HEADER_TAG_MODULE_ALIGN,
		MULTIBOOT_HEADER_TAG_OPTIONAL,
		sizeof(header.align)
	},
	// {
	// 	MULTIBOOT_HEADER_TAG_ADDRESS,
	// 	MULTIBOOT_HEADER_TAG_OPTIONAL,
	// 	sizeof(struct multiboot_header_tag_address),
	// 	(uint32_t)&header,
	// 	0x00100000u,
	// 	(uint32_t)edata,
	// 	(uint32_t)end		
	// },
	// {
	// 	MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS,
	// 	MULTIBOOT_HEADER_TAG_OPTIONAL,
	// 	sizeof(struct multiboot_header_tag_address),
	// 	(uint32_t)start,
	// },
#ifdef ENABLE_FRAME_BUFFER	
	{
		MULTIBOOT_HEADER_TAG_FRAMEBUFFER,
		MULTIBOOT_HEADER_TAG_OPTIONAL,
		sizeof(header.frame),
		80,
		25,
		16
	},
#endif // ENABLE_FRAME_BUFFER
	{	/* end */
		MULTIBOOT_TAG_TYPE_END,
		0,
		sizeof(header.end)
	}
 };
