[BITS 32]

[GLOBAL gdt_flush]    ; Allows the C code to call gdt_flush().

[GLOBAL idt_flush]    ; Allows the C code to call idt_flush().

gdt_flush:
	mov eax, [esp+4]  ; Get the pointer to the GDT, passed as a parameter.
	lgdt [eax]        ; Load the new GDT pointer

	mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
	mov ds, ax        ; Load all data segment selectors
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:.flush   ; 0x08 is the offset to our code segment: Far jump!
.flush:
	ret


gdt_flush64:
	mov eax, [esp+4] ; Get the pointer to the GDT, passed as a parameter.
	lgdt [eax]        ; Load the new GDT pointer

	mov ax, 0x10  ; 0x10 is the offset in the GDT to our data segment
	mov ds, ax        		; Load all data segment selectors
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:long_mode_start   ; 0x08 is the offset to our code segment: Far jump!
.flush:
	ret


idt_flush:
	mov eax, [esp+4]  ; Get the pointer to the IDT, passed as a parameter. 
	lidt [eax]        ; Load the IDT pointer.
	ret
   


section .text
[BITS 64]

[GLOBAL gdt_flush64]

[EXTERN kmain64]

long_mode_start:
	mov rax, 0x2f592f412f4b2f4f
	mov qword [0xb8000], rax
	mov qword [0xb8008], rax
	mov qword [0xb8010], rax
	mov qword [0xb8018], rax
	mov qword [0xb8020], rax
	jmp kmain64
.loop:
	hlt
	jmp .loop
