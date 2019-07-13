[BITS 32]                       ; All instructions should be 32-bit.

[GLOBAL outb]
[GLOBAL outw]
[GLOBAL inb]
[GLOBAL inw]
[GLOBAL idle_loop]
[GLOBAL set_page_directory]
[GLOBAL get_fault_addr]

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

idle_loop:
loop:
	hlt
	jmp loop


set_page_directory:
	mov eax, [esp+4]
	mov	cr3, eax
	mov eax, cr0
	or eax,80000000h
	mov cr0, eax
	ret

get_fault_addr:
	mov eax, cr2
	ret
