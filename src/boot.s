section .text
[BITS 32]

[GLOBAL start]
[GLOBAL gdt_flush]
[GLOBAL mboot_header]
[EXTERN init_gdt]
[EXTERN kmain64]
[EXTERN kernel_stack];

%include "macros.mac"

; on entry, ebx holds the multi-boot header pointer
; eax holds the magic value

start:
	cli
	cmp eax,MULTIBOOT2_BOOTLOADER_MAGIC
	jnz  .error
	mov esp, kernel_stack	; setup a stack
	mov edi, eax	; save magic value into edi - will become rdi in x64 == first argument
	mov esi, ebx	; save multiboot header pointer in esi - will become rsi on x64 == second argument
.load_p4_table:
	; move page table address to cr3
	mov eax, p4_table
	mov cr3, eax

	; enable PAE
.enable_pae:
	mov eax, cr4
	or eax, 1 << 5
	mov cr4, eax

	; set the long mode bit
.set_long_mode:
	mov ecx, 0xC0000080
	rdmsr
	or eax, 1 << 8
	wrmsr

	; enable paging
.enable_paging:
	mov eax, cr0
	or eax, 1 << 31
	or eax, 1 << 16
	mov cr0, eax
.gdt_flush:
	mov eax, gdt
	lgdt [eax]        ; Load the new GDT pointer
	mov ax, 0x10  ; 0x10 is the offset in the GDT to our data segment
	mov ds, ax        		; Load all data segment selectors
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	push esi
	push 0
	push edi
	push 0
.jmp_start64:
	jmp 0x08:start64   ; 0x08 is the offset to our code segment: Far jump!
.loop:
	hlt				; should never get here
.error:
	xchg bx,bx
	jmp .loop

[BITS 64]
start64:
	and edi, edi	; 32-bit magic value is in edi. Make sure high order DWORD of rdi is cleared
	and esi,esi	; multiboot header pointer was put in esi. It's < 32 bits, make sure upper DWORD of rsi is cleared.
	jmp kmain64	; Jump to kmain64
	xchg bx,bx	
.loop:
	hlt				; should never get here
	jmp .loop	

;
;	The multiboot header
;

align 8
mboot_header:
	dd MULTIBOOT2_HEADER_MAGIC
	dd MULTIBOOT_ARCHITECTURE_I386
	dd (.console_tag-mboot_header)
	dd (0x100000000 - (MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + (.console_tag-mboot_header)))
align 8
.console_tag:
	MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS
	MULTIBOOT_HEADER_TAG_OPTIONAL
	dd (.module_align_tag-.console_tag)
	MULTIBOOT_CONSOLE_FLAGS_EGA_TEXT_SUPPORTED
align 8
.module_align_tag:
	MULTIBOOT_HEADER_TAG_MODULE_ALIGN
	MULTIBOOT_HEADER_TAG_OPTIONAL
	dd (.end_tag-.module_align_tag)
align 8
.end_tag:
	MULTIBOOT_HEADER_TAG_END
	dw 0
	dd (.mboot_header_end-.end_tag)
.mboot_header_end:


section .data
; Our GDT

align 0x10
entries:
.null:
	dq 0
.code:
	GDT 0, 0xFFFFFFFF, (GDT_CODE_SEGMENT|GDT_READ_WRITE|GDT_CONFORMING|GDT_DESCRIPTOR_CODE_DATA|GDT_RING0|GDT_PRESENT|GDT_64_BIT_OPERAND|GDT_GRAN_4K)
.data:
	GDT 0, 0xFFFFFFFF, (GDT_DATA_SEGMENT|GDT_READ_WRITE|GDT_DIRECTION_UP|GDT_DESCRIPTOR_CODE_DATA|GDT_RING0|GDT_PRESENT|GDT_64_BIT_OPERAND|GDT_GRAN_4K)
.user_code:
	GDT 0, 0xFFFFFFFF, (GDT_CODE_SEGMENT|GDT_READ_WRITE|GDT_CONFORMING|GDT_DESCRIPTOR_CODE_DATA|GDT_RING3|GDT_PRESENT|GDT_64_BIT_OPERAND|GDT_GRAN_4K)
.user_data:
	GDT 0, 0xFFFFFFFF, (GDT_DATA_SEGMENT|GDT_READ_WRITE|GDT_DIRECTION_UP|GDT_DESCRIPTOR_CODE_DATA|GDT_RING3|GDT_PRESENT|GDT_64_BIT_OPERAND|GDT_GRAN_4K)
.user_code_32:
	GDT 0, 0xFFFFFFFF, (GDT_CODE_SEGMENT|GDT_READ_WRITE|GDT_CONFORMING|GDT_DESCRIPTOR_CODE_DATA|GDT_RING3|GDT_PRESENT|GDT_32_BIT_OPERAND|GDT_GRAN_4K)
.user_data_32:
	GDT 0, 0xFFFFFFFF, (GDT_DATA_SEGMENT|GDT_READ_WRITE|GDT_DIRECTION_UP|GDT_DESCRIPTOR_CODE_DATA|GDT_RING3|GDT_PRESENT|GDT_32_BIT_OPERAND|GDT_GRAN_4K)
.end_entries:

gdt:
	dw (entries.end_entries - entries)
	dq entries
.end_gdt:

; Create initial identity mapped page table mapping the first 1GB of physical RAM
; the the first 1GB of virtual address space (i.e. virtual address == physical address )
align 0x1000
p4_table:			; PML4E
	dq (p3_table + 3)
	TIMES 511 dq 0
p3_table:			; PDPTE
	dq (p2_table + 3)
	TIMES 511 dq 0
p2_table:						; PDE with (PS=1 == 2mb pages, PS == 0 -> points to PTE)
	%assign p 0x83			; set the PS bit (0x80), the rw & present bits (0x03) 
	%rep 511					; write 511 of 512 entries to the PDE table
	dq p						; write page entry
	%assign p p+ 0x00200000	; increment page entry virtual ( and physical ) address by page size (2MB)
	%endrep					; end of loop
	dq (p4_table + 3)			; recursive last table entry points back to p4_table


