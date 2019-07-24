[BITS 64]                       ; All instructions should be 32-bit.

[GLOBAL outb]
[GLOBAL outw]
[GLOBAL outd]
[GLOBAL inb]
[GLOBAL inw]
[GLOBAL ind]
; [GLOBAL start]
[GLOBAL get_fault_addr64]
[GLOBAL idle_loop]
[EXTERN kmain64]
; [GLOBAL cursor_x]
; [GLOBAL cursor_y]
; [GLOBAL backColor]
; [GLOBAL foreColor]
; [GLOBAL cur_line]
; [GLOBAL back_buffer]

; back_buffer	times 2048	dw	0
; cur_line		dd	0x00000000
; cursor_x 		dw	0x0000
; cursor_y 		dw	0x0000
; backColor 	db	0x00
; foreColor 		db	0x03

; start:
	; call kmain64
; .loop:
	; hlt
	; jmp .loop


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
	
idle_loop:
	hlt
	jmp idle_loop