#if 0
#include "multiboot2.h"

extern "C"
{
	struct alignas(8)  MultiBoot
	{
		alignas(8)  multiboot_header header;
		alignas(8)  multiboot_header_tag_console_flags console;
		alignas(8)  multiboot_header_tag_module_align align;
	#ifdef ENABLE_FRAME_BUFFER		
		alignas(8)  multiboot_header_tag_framebuffer frame;
	#endif	
		alignas(8)  multiboot_header_tag end;
	};


#pragma clang section data=".text" rodata=".text" 
#pragma clang optimize off
	
	extern MultiBoot header;
	
	alignas(8) MultiBoot header = {
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
			MULTIBOOT_HEADER_TAG_END,
			0,
			sizeof(header.end)
		}
	 };
	 
#pragma clang optimize on
#pragma clang section data="" rodata="" 
	 
 }
 #endif // 0
 