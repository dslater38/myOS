;
; boot.s -- Kernel start location. Also defines multiboot header.
; Based on Bran's kernel development tutorial file start.asm
;

; MBOOT_PAGE_ALIGN    equ 1<<0    ; Load kernel and modules on a page boundary
; MBOOT_MEM_INFO      equ 1<<1    ; Provide your kernel with memory info
; MBOOT_HEADER_MAGIC  equ 0x1BADB002 ; Multiboot Magic value
; MBOOT2_HEADER_MAGIC  equ  0xe85250d6 ; Multiboot2 Magic value
; NOTE: We do not use MBOOT_AOUT_KLUDGE. It means that GRUB does not
; pass us a symbol table.
; MBOOT_HEADER_FLAGS  equ ( MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO )
; MBOOT_CHECKSUM      equ ( -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS) )


[BITS 32]                       ; All instructions should be 32-bit.

[EXTERN bss]                    ; Start of the .bss section.
[EXTERN end]                    ; End of the last loadable section.

[GLOBAL start]                  ; Kernel entry point.
[EXTERN kmain32]                   ; This is the entry point of our C code
[EXTERN placement_address]
[EXTERN init_stack32]

start:
	cli                   					; Disable interrupts.
	mov	esp, end					; setup the stack and kmalloc's placement_address
	add esp, 0x1000				; add a page padding
	mov [placement_address], esp;	
	mov esp, init_stack32
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


  ; push    ebx                   ; Load multiboot header location
  ; push    eax			; push the magic value.

  ; Execute the kernel:
  ; cli                         ; Disable interrupts.
  ; call kmain                   ; call our main() function.
; loop:
   ; hlt
  ; jmp loop                      ; Enter an infinite loop, to stop the processor
                              ; executing whatever rubbish is in the memory
                              ; after our kernel!
