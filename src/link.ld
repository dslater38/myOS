/* Link.ld -- Linker script for the kernel - ensure everything goes in the */
/*            Correct place.  */
/*            Original file taken from Bran's Kernel Development */
/*            tutorials: http://www.osdever.net/bkerndev/index.php. */

ENTRY(start)
SECTIONS
{
	.text 0x00120000 :
	{
		code = .; _code = .; __code = .;
		*(.text)
		. = ALIGN(4096);
	}

	.data :
	{
		data = .; _data = .; __data = .;
		*(.data)
		*(.rodata)
		. = ALIGN(4096);
	}
  
	.stack :
	{
		. += 16384; 
		. = ALIGN(4096);
		init_stack32 = .; _init_stack32 = .; __init_stack32 = .;
	}
	.bss :
	{
		bss = .; _bss = .; __bss = .;
		*(.bss)
		. = ALIGN(4096);
	}
  
	end = .; _end = .; __end = .;
  
}

