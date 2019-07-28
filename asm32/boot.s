[BITS 32]

[GLOBAL start]
[GLOBAL gdt_flush]
[EXTERN init_gdt]
[EXTERN kmain64]
[EXTERN init_stack32];

%define MULTIBOOT2_BOOTLOADER_MAGIC  0x36d76289

; on entry, ebx holds the multi-boot header pointer
; eax holds the magic value

start:
	cli
	cmp eax,MULTIBOOT2_BOOTLOADER_MAGIC
	jnz  .error
	mov esp, init_stack32	; setup a stack
	mov edi, eax	; save magic value into edi - will become rdi in x64 == first argument
	mov esi, ebx	; save multiboot header pointer in esi - will become rsi on x64 == second argument
.enable_paging:
	; move page table address to cr3
	mov eax, p4_table
	mov cr3, eax

	; enable PAE
	mov eax, cr4
	or eax, 1 << 5
	mov cr4, eax

	; set the long mode bit
	mov ecx, 0xC0000080
	rdmsr
	or eax, 1 << 8
	wrmsr

	; enable paging
	mov eax, cr0
	or eax, 1 << 31
	or eax, 1 << 16
	mov cr0, eax
.end_paging
	mov eax, gdt
.gdt_flush:
	lgdt [eax]        ; Load the new GDT pointer

	mov ax, 0x10  ; 0x10 is the offset in the GDT to our data segment
	mov ds, ax        		; Load all data segment selectors
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:kmain64   ; 0x08 is the offset to our code segment: Far jump!
.loop:
	hlt				; should never get here
.error:
	xchg bx,bx
	jmp .loop

; gdt_flush:
;	mov eax, [esp+4] ; Get the pointer to the GDT, passed as a parameter.
	; lgdt [eax]        ; Load the new GDT pointer

	; mov ax, 0x10  ; 0x10 is the offset in the GDT to our data segment
	; mov ds, ax        		; Load all data segment selectors
	; mov es, ax
	; mov fs, ax
	; mov gs, ax
	; mov ss, ax
	; jmp 0x08:kmain64   ; 0x08 is the offset to our code segment: Far jump!
	; ret						; should never get here


; enable_paging:
;	move page table address to cr3
	; mov eax, p4_table
	; mov cr3, eax

;	enable PAE
	; mov eax, cr4
	; or eax, 1 << 5
	; mov cr4, eax

;	set the long mode bit
	; mov ecx, 0xC0000080
	; rdmsr
	; or eax, 1 << 8
	; wrmsr

;	enable paging
	; mov eax, cr0
	; or eax, 1 << 31
	; or eax, 1 << 16
	; mov cr0, eax
	; ret


entries:
	dq 0x0000000000000000
	dq 0x00af9e000000ffff
	dq 0x00af92000000ffff
	dq 0x00affe000000ffff
	dq 0x00aff2000000ffff
	dq 0x00cffe000000ffff
	dq 0x00cff2000000ffff
end_entries

gdt:
	dw (end_entries - entries)
	dq entries

align 4096

p4_table:			; PML4E
	dq (p3_table + 3)
	TIMES 511 dq 0
p3_table:			; PDPTE
	dq (p2_table + 3)
	TIMES 511 dq 0
p2_table:			; PDE with (PS=1 == 2mb pages, PS == 0 -> points to PTE)
	%assign p 0x83
	%rep 512
	dq p
	%assign p p+ 0x00200000
	%endrep
