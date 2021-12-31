/* Link.ld -- Linker script for the kernel - ensure everything goes in the */
/*            Correct place.  */
/*            Original file taken from Bran's Kernel Development */
/*            tutorials: http://www.osdever.net/bkerndev/index.php. */

OUTPUT_FORMAT(elf64-x86-64)
ENTRY(start)



VM_BASE = 0xFFFF800000000000;
/* VM_BASE = 0x0000000000000000; */
/* VM_BASE = 0x00000000C0000000; */

SECTIONS
{
	. = 0x00100000;

	__kernel_start = .;

	.multiboot.data : {
		*(.multiboot.data)
	}

	.multiboot.text : {
		*(.multiboot.text)
	}

	. += VM_BASE;


	kernel_begin = .; _kernel_begin = .; __kernel_begin = .;

	.text ALIGN (4K) : AT (ADDR (.text) - VM_BASE)
	{
		code = .; _code = .; __code = .;
/*		src/boot.o(.text) */
		*(.text*)
	}
	.rodata ALIGN (4K) : AT (ADDR (.rodata) - VM_BASE)
	{
		rodata = .; _rodata = .; __rodata = .;
		*(.rodata*)
	}
	.data ALIGN (4K) : AT (ADDR (.data) - VM_BASE)
	{
		data = .; _data = .; __data = .;
		*(.data*)
	}
	.bss ALIGN (4K) : AT (ADDR (.bss) - VM_BASE)
	{
		bss = .; _bss = .; __bss = .;
		*(.bss*)
	}
	/* . = ALIGN(CONSTANT (MAXPAGESIZE)); */
    .init ALIGN (4K)  : AT (ADDR (.init) - VM_BASE)
    {
		KEEP (*(SORT_NONE(.init)))
    }
	.fini           : AT (ADDR (.fini) - VM_BASE)
    {
		KEEP (*(SORT_NONE(.fini)))
    }
	.preinit_array : AT (ADDR (.preinit_array) - VM_BASE)
	{
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP (*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);
	}
	.init_array : AT (ADDR (.init_array) - VM_BASE)
	{
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
		KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
		PROVIDE_HIDDEN (__init_array_end = .);
	}
	.fini_array : AT (ADDR (.fini_array) - VM_BASE)
	{
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
		KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
		PROVIDE_HIDDEN (__fini_array_end = .);
	} 
	.ctors ALIGN (4K) : AT (ADDR (.ctors) - VM_BASE)
	{
		/* gcc uses crtbegin.o to find the start of
		the constructors, so we make sure it is
		first.  Because this is a wildcard, it
		doesn't matter if the user does not
		actually link against crtbegin.o; the
		linker won't look for a file to match a
		wildcard.  The wildcard also means that it
		doesn't matter which directory crtbegin.o
		is in.  */
		KEEP (*crtbegin.o(.ctors))
		KEEP (*crtbegin?.o(.ctors))
		/* We don't want to include the .ctor section from
		the crtend.o file until after the sorted ctors.
		The .ctor section from the crtend file contains the
		end of ctors marker and it must be last */
		KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
		KEEP (*(SORT(.ctors.*)))
		KEEP (*(.ctors))
	}
	.dtors ALIGN (4K) : AT (ADDR (.dtors) - VM_BASE)
	{
		KEEP (*crtbegin.o(.dtors))
		KEEP (*crtbegin?.o(.dtors))
		KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
		KEEP (*(SORT(.dtors.*)))
		KEEP (*(.dtors))
	}
	.stack ALIGN (4K) : AT (ADDR (.stack) - VM_BASE)
	{
		kernel_stack_base = .; _kernel_stack_base = .; __kernel_stack_base = .; 
		. += 16384; 
		. = ALIGN(4096);
		__kernel_stack = .;
		_kernel_stack = .; 
		kernel_stack = .;
	}

/*	end = .; _end = .; __end = .; */
	kernel_end = .; _kernel_end = .; __kernel_end = .;
  
}

