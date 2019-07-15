[BITS 64]                       ; All instructions should be 32-bit.

[GLOBAL outb64]
[GLOBAL outw64]
[GLOBAL inb64]
[GLOBAL inw64]


outb64:
	mov rdx, [rsp+4]
	mov rax, [rsp+8]
	out dx, al
	ret
	
outw64:
	mov rdx, [rsp+4]
	mov rax, [rsp+8]
	out dx, ax
	ret

inb64:
	mov rdx, [rsp+4]
	in al, dx
	ret


inw64:
	mov rdx, [rsp+4]
	in ax, dx
	ret

