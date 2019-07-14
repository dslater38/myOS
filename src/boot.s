[BITS 32]                       ; All instructions should be 32-bit.

[EXTERN bss]                    ; Start of the .bss section.
[EXTERN end]                    ; End of the last loadable section.

[GLOBAL start]                  ; Kernel entry point.
[EXTERN kmain32]                   ; This is the entry point of our C code
[EXTERN placement_address]
[EXTERN init_stack32]

start:
	cli                   					; Disable interrupts.
	mov	esp, end					; setup kmalloc's placement_address 1 page beyong end
	add esp, 0x1000				; 
	mov [placement_address], esp;	;
	mov esp, init_stack32			; set up the initial stack. init_stack32 defines  16K stack
	and	esp, 0xFFFFF000			; make sure it starts on a page boundary.
	push	0h					; reset EFLAGS
	popf						;
	push	ebx           			; Load multiboot header location
	push	eax					; push the magic value.
								; Execute the kernel:
	call	kmain32           				; call our main() function.
	; Enter an infinite loop, to stop the processor  executing whatever rubbish is in the memory after our kernel!
idle_loop:
	hlt
	jmp idle_loop
