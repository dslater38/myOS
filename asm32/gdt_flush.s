[BITS 32]

[GLOBAL gdt_flush64]
gdt_flush64:
	mov eax, [esp+4] ; Get the pointer to the GDT, passed as a parameter.
	mov edx, [esp+8]
	lgdt [eax]        ; Load the new GDT pointer

	mov ax, 0x10  ; 0x10 is the offset in the GDT to our data segment
	mov ds, ax        		; Load all data segment selectors
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	push 0x08
	push edx
	retf
	
;	jmp 0x08:long_mode_start   ; 0x08 is the offset to our code segment: Far jump!
;	ret

[GLOBAL enter_x64_mode]


section .text
[BITS 64]

[GLOBAL idt_flush64]
[EXTERN kmain64]

long_mode_start:
	mov rax, 0x2f592f412f4b2f4f
	mov qword [0xb8000], rax
	mov rax,0xFFFFFFFF00000000
	and rdx, rax
	call [rdx]
.loop:
	hlt
	jmp .loop


idt_flush64:
	lidt [rdi]        ; Load the IDT pointer.
	ret
