[BITS 64]                       ; All instructions should be 32-bit.

[GLOBAL outb64]
[GLOBAL outw64]
[GLOBAL inb64]
[GLOBAL inw64]
[GLOBAL get_fault_addr64]
[GLOBAL idle_loop]


outb64:
	mov rdx, rdi
	mov rax, rsi
	out dx, al
	ret
	
outw64:
	mov rdx, rdi
	mov rax, rsi
	out dx, ax
	ret

inb64:
	mov rdx, rdi
	in al, dx
	ret

inw64:
	mov rdx, rdi
	in ax, dx
	ret

get_fault_addr64:
	mov rax, cr2
	ret
	
idle_loop:
	hlt
	jmp idle_loop