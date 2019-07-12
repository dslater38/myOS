;
; boot.s -- Kernel start location. Also defines multiboot header.
; Based on Bran's kernel development tutorial file start.asm
;

MBOOT_PAGE_ALIGN    equ 1<<0    ; Load kernel and modules on a page boundary
MBOOT_MEM_INFO      equ 1<<1    ; Provide your kernel with memory info
MBOOT_HEADER_MAGIC  equ 0x1BADB002 ; Multiboot Magic value
MBOOT2_HEADER_MAGIC  equ  0xe85250d6 ; Multiboot2 Magic value
; NOTE: We do not use MBOOT_AOUT_KLUDGE. It means that GRUB does not
; pass us a symbol table.
MBOOT_HEADER_FLAGS  equ ( MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO )
MBOOT_CHECKSUM      equ ( -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS) )


[BITS 32]                       ; All instructions should be 32-bit.

[GLOBAL mboot]                  ; Make 'mboot' accessible from C.
[GLOBAL mboot2]                  ; Make 'mboot' accessible from C.
[GLOBAL mboot2_end]
[EXTERN code]                   ; Start of the '.text' section.
[EXTERN bss]                    ; Start of the .bss section.
[EXTERN end]                    ; End of the last loadable section.

struc mbootType
	.magic:			resd	1
	.flags:			resd	1
	.checksum:		resd	1
	.header_addr:		resd	1
	.load_addr:		resd	1
	.load_end_addr:	resd	1
	.bss_end_addr:		resd	1
	.entry_addr:		resd	1
	.mode_type:		resd	1
	.width:			resd 1
	.height:			resd 1
	.depth:			resd 1

endstruc

struc mboot2Type
	.magic:			resd 1
	.mode:			resd 1
	.header_length:	resd 1
	.checksum:		resd 1
	.type:			resw 1
	.flags:			resw 1
	.size:			resw 1
endstruc

mboot:
	istruc mbootType
		at mbootType.magic, 			dd  MBOOT_HEADER_MAGIC	; GRUB will search for this value on each
		at mbootType.flags, 			dd  MBOOT_HEADER_FLAGS	; How GRUB should load your file / settings
		at mbootType.checksum,		dd  MBOOT_CHECKSUM		; To ensure that the above values are correct
		at mbootType.header_addr,		dd  mboot				; Location of this descriptor
		at mbootType.load_addr, 		dd  code					; Start of kernel '.text' (code) section.
		at mbootType.load_end_addr,	dd  bss					; End of kernel '.data' section.
		at mbootType.bss_end_addr, 	dd  end					; End of kernel.
		at mbootType.entry_addr, 		dd  start					; Kernel entry point (initial EIP).
		at mbootType.mode_type, 		dd 0						; video mode (ignored - flag not set)
		at mbootType.width, 			dd 0						; video width (ignored - flag not set)
		at mbootType.height, 			dd 0						; video height (ignored - flag not set)
		at mbootType.depth, 			dd 0						; video depth (ignored - flag not set)
	iend
mboot2:
	istruc mboot2Type
		at mboot2Type.magic,			dd MBOOT2_HEADER_MAGIC
		at mboot2Type.mode,			dd 0
		at mboot2Type.header_length,	dd 0x100000000 - (0xe85250d6 + 0 + (mboot2_end - mboot2))
		at mboot2Type.type,			dw 0
		at mboot2Type.flags,			dw 0
		at mboot2Type.size,			dw 0
	iend
mboot2_end:

[GLOBAL start]                  ; Kernel entry point.
[EXTERN kmain]                   ; This is the entry point of our C code

start:
  push    ebx                   ; Load multiboot header location
  push    eax			; push the magic value.

  ; Execute the kernel:
  cli                         ; Disable interrupts.
  call kmain                   ; call our main() function.
loop:
   hlt
  jmp loop                      ; Enter an infinite loop, to stop the processor
                              ; executing whatever rubbish is in the memory
                              ; after our kernel!
