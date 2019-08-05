section .text
[BITS 64]                       ; All instructions should be 32-bit.
[GLOBAL outb]
[GLOBAL outw]
[GLOBAL outd]
[GLOBAL inb]
[GLOBAL inw]
[GLOBAL ind]

[GLOBAL get_fault_addr64]
[GLOBAL halt]
[GLOBAL invalidate_tlb]
[GLOBAL invalidate_all_tlbs]

invalidate_tlb:
	invlpg [rdi]
	ret

invalidate_all_tlbs:
	mov rax, cr3
	mov cr3, rax
	ret

outb:
	mov rdx, rdi
	mov rax, rsi
	out dx, al
	ret
	
outw:
	mov rdx, rdi
	mov rax, rsi
	out dx, ax
	ret
	
outd:
	mov rdx, rdi
	mov rax, rsi
	out dx, eax
	ret

inb:
	mov rdx, rdi
	in al, dx
	ret

inw:
	mov rdx, rdi
	in ax, dx
	ret

ind:
	mov rdx, rdi
	in eax, dx
	ret

get_fault_addr64:
	mov rax, cr2
	ret
	
halt:
	cli
	hlt
	jmp halt