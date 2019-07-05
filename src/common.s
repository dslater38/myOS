[BITS 32]                       ; All instructions should be 32-bit.
[GLOBAL outb]
[GLOBAL outw]
[GLOBAL inb]
[GLOBAL inw]

outb:
	mov edx, [esp+4]
	mov eax, [esp+8]
	out dx, al
	ret
	
outw:
	mov edx, [esp+4]
	mov eax, [esp+8]
	out dx, ax
	ret

inb:
	mov edx, [esp+4]
	in al, dx
	ret


inw:
	mov edx, [esp+4]
	in ax, dx
	ret
