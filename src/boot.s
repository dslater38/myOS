section .text
[BITS 32]

[GLOBAL start]
[GLOBAL gdt_flush]
[GLOBAL mboot_header]
[GLOBAL p4_table]
[GLOBAL p3_table]
[GLOBAL p2_table]
[GLOBAL p3_gb_mapped_table]
[GLOBAL startup_data_start]
[GLOBAL startup_data_end]
[EXTERN init_gdt]
[EXTERN kmain64]
[EXTERN kernel_stack];
[EXTERN init_idt64_table]
[EXTERN __libc_init_array]

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
	mov ebp, start
	push ebp				; set up a fake initial stack frame 
	push 0					; make it look like a 64-bit frame
	mov ebp, esp			; that looks like a call to start
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
	pop ebp			; undo the 2 pushes at the start
	pop ebp			; first pop is high-order DWORD == 0 - discard

[BITS 64]
start64:
	push start.loop	; setup fake return address - allows debugger to walk up the stack
	mov rbp, rsp
	and edi, edi	; 32-bit magic value is in edi. Make sure high order DWORD of rdi is cleared
	and esi, esi	; multiboot header pointer was put in esi. It's < 32 bits, make sure upper DWORD of rsi is cleared.
	mov r14, rdi	; save rdi & rsi while we call init_idt64_table & __libc_init_array
	mov r15, rsi
	call init_idt64_table
	call __libc_init_array
	mov rdi, r14
	mov rsi, r15
	call kmain64	; Jump to kmain64
	xchg bx,bx	
.loop:
	hlt				; should never get here
	jmp .loop
	pop rbp			; will never get here

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
startup_data_start:
entries:
.null:				; null entry (required)
	dq 0
.code:				; ring 0 kernel code (64-bit)
	GDT 0, 0xFFFFFFFF, (GDT_CODE_SEGMENT|GDT_READ_WRITE|GDT_CONFORMING|GDT_DESCRIPTOR_CODE_DATA|GDT_RING0|GDT_PRESENT|GDT_64_BIT_OPERAND|GDT_GRAN_4K)
.data:				; ring 0 kernel data (64-bit)
	GDT 0, 0xFFFFFFFF, (GDT_DATA_SEGMENT|GDT_READ_WRITE|GDT_DIRECTION_UP|GDT_DESCRIPTOR_CODE_DATA|GDT_RING0|GDT_PRESENT|GDT_64_BIT_OPERAND|GDT_GRAN_4K)
.user_code_32:		; ring 3 user code (32-bit compatibility mode)
	GDT 0, 0xFFFFFFFF, (GDT_CODE_SEGMENT|GDT_READ_WRITE|GDT_CONFORMING|GDT_DESCRIPTOR_CODE_DATA|GDT_RING3|GDT_PRESENT|GDT_32_BIT_OPERAND|GDT_GRAN_4K)
.user_data_32:		; ring 3 user data (32-bit compatibility mode)
	GDT 0, 0xFFFFFFFF, (GDT_DATA_SEGMENT|GDT_READ_WRITE|GDT_DIRECTION_UP|GDT_DESCRIPTOR_CODE_DATA|GDT_RING3|GDT_PRESENT|GDT_32_BIT_OPERAND|GDT_GRAN_4K)
.user_code:			; ring 3 user code (64-bit)
	GDT 0, 0xFFFFFFFF, (GDT_CODE_SEGMENT|GDT_READ_WRITE|GDT_CONFORMING|GDT_DESCRIPTOR_CODE_DATA|GDT_RING3|GDT_PRESENT|GDT_64_BIT_OPERAND|GDT_GRAN_4K)
.user_data:			; ring 3 user data (64-bit)
	GDT 0, 0xFFFFFFFF, (GDT_DATA_SEGMENT|GDT_READ_WRITE|GDT_DIRECTION_UP|GDT_DESCRIPTOR_CODE_DATA|GDT_RING3|GDT_PRESENT|GDT_64_BIT_OPERAND|GDT_GRAN_4K)
.user_code_16:		; ring 3 user code (16-bit protected (not used))
	GDT 0, 0xFFFFFFFF, (GDT_CODE_SEGMENT|GDT_READ_WRITE|GDT_CONFORMING|GDT_DESCRIPTOR_CODE_DATA|GDT_RING3|GDT_PRESENT|GDT_16_BIT_OPERAND|GDT_GRAN_4K)
.user_data_16:		; ring 3 user date (16-bit protected (not used))
	GDT 0, 0xFFFFFFFF, (GDT_DATA_SEGMENT|GDT_READ_WRITE|GDT_DIRECTION_UP|GDT_DESCRIPTOR_CODE_DATA|GDT_RING3|GDT_PRESENT|GDT_16_BIT_OPERAND|GDT_GRAN_4K)	
.end_entries:

gdt:
	dw (entries.end_entries - entries)
	dq entries
.end_gdt:

; Create initial identity mapped page table mapping the first 1GB of physical RAM
; the the first 1GB of virtual address space (i.e. virtual address == physical address )
align 0x1000
p4_table:							; PML4E
	dq (p3_table + 3)				; pointer to PDPTE table with rw & present bits set
	TIMES 510 dq 0					; write 510 null entries to fill the table.
	dq (p4_table + 3)				; write recursive entry
p3_table:							; PDPTE
	dq (p2_table + 3)				; pointer to PDE table with rw & present bits set.
	TIMES 511 dq 0					; write 511 null entries to fill the table.
p2_table:							; PDE 
	dq (p1_table + 0x0003)			; pointer to the first PTE table with rw & present bits set
	dq (p1_table + 0x1003)			; pointer to the second PTE table with rw & present bits set
	dq (p1_table + 0x2003)			; pointer to the third PTE table with rw & present bits set
	dq (p1_table + 0x3003)			; pointer to the forth PTE table with rw & present bits set
	TIMES 508 dq 0					; write 511 null entries for a total of 512 entries
p1_table:							; PTE 
	%assign p 0x0000000000000003	; set the rw & present bits (0x03) 
	%rep 2048						; write 2048 entries to 4 PTE tables to cover 8 MB physical RAM
	dq p							; write page entry
	%assign p p + 0x0000000000001000; increment page entry virtual ( and physical ) address by page size (4K)
	%endrep							; end of loop
p3_gb_mapped_table:
	%assign p 0x0000000000000183	; set the rw, present, and global bits (0x03) 
	%rep 512						; write 512 entries 1GB page entries to the table. set the global,rw,present & huge bits
	dq p							; write page entry
	%assign p p + 0x0000000040000000; increment page entry virtual ( and physical ) address by page size (1GB) for first 512 GB of RAM
	%endrep							; end of loop
startup_data_end:


