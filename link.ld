/* Link.ld -- Linker script for the kernel - ensure everything goes in the */
/*            Correct place.  */
/*            Original file taken from Bran's Kernel Development */
/*            tutorials: http://www.osdever.net/bkerndev/index.php. */

OUTPUT_FORMAT(elf64-x86-64)
ENTRY(start)
SECTIONS
{	
	.text 0x100000 :
	{
		code = .; _code = .; __code = .;
		src/boot.o(.text)
		*(.text)
		. = ALIGN(4096);
	}

	.data :
	{
		data = .; _data = .; __data = .;
		*(.data)
		. = ALIGN(4096);
	}
	.rodata :
	{
		rodata = .; _rodata = .; __rodata = .;
		*(.rodata)
		. = ALIGN(4096);
	}
	.bss :
	{
		bss = .; _bss = .; __bss = .;
		*(.bss)
		. = ALIGN(4096);
	}
 
	.stack :
	{
		kernel_stack_base = .; _kernel_stack_base = .; __kernel_stack_base = .; 
		. += 16384; 
		. = ALIGN(4096);
		kernel_stack = .; _kernel_stack = .; __kernel_stack = .;
		. = ALIGN(4096);
	}
	. = ALIGN(CONSTANT (MAXPAGESIZE));
    .init           :
	{
		KEEP (*(SORT_NONE(.init)))
	}
		
	.fini           :
	{
		KEEP (*(SORT_NONE(.fini)))
	}
	.preinit_array    :
	{
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP (*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);
	}
	.init_array    :
	{
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
		KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
		PROVIDE_HIDDEN (__init_array_end = .);
	}
	.fini_array    :
	{
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
		KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
		PROVIDE_HIDDEN (__fini_array_end = .);
	}
 
	.ctors :
	{
      __CTOR_LIST__ = .;
      LONG((__CTOR_END__ - __CTOR_LIST__) / 8 - 2)
      KEEP(*(.ctors))
      LONG(0)
      __CTOR_END__ = .;
	}
	.dtors :
	{
      __DTOR_LIST__ = .;
      QUAD((__DTOR_END__ - __DTOR_LIST__) / 8 - 2)
      KEEP(*(.dtors))
      QUAD(0)
      __DTOR_END__ = .;  

	  . = ALIGN(4096);
	}

	end = .; _end = .; __end = .;
  
}

