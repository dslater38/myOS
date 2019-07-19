section .text
[BITS 64]

[GLOBAL idt_flush]
[GLOBAL long_mode_start]
[EXTERN kmain64]


long_mode_start:
	mov rax, 0x2f592f412f4b2f4f
	mov qword [0xb8000], rax
	call kmain64
.loop:
	hlt
	jmp .loop


idt_flush:
	lidt [rdi]        ; Load the IDT pointer.
	ret
