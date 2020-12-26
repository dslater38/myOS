section .text
[BITS 32]

[GLOBAL start]
; [GLOBAL gdt_flush]
[GLOBAL mboot_header]
[GLOBAL p4_table]
[GLOBAL p3_table]
[GLOBAL p2_table]
[GLOBAL startup_data_start]
[GLOBAL startup_data_end]
[EXTERN init_gdt]
[EXTERN kmain64]
[EXTERN kernel_stack];
[EXTERN init_idt64_table]
[EXTERN __libc_init_array]

%include "macros.mac"

%define CODE64 (entries.code-entries)
%define DATA64 (entries.data-entries)
%define USERCODE64 (entries.user_code-entries)
%define USERDATA64 (entries.user_data-entries)
%define USERCODE32 (entries.user_code_32-entries)
%define USERDATA32 (entries.user_data_32-entries)
%define USERCODE16 (entries.user_code_16-entries)
%define USERDATA16 (entries.user_data_16-entries)
; %define VM_BASE 0xFFFF800000000000
; %define VM_BASE 0x0000000000000000
%define VM_BASE 0x00000000C0000000


section .multiboot.text

; on entry, ebx holds the multi-boot header pointer
; eax holds the magic value
align 16
start:
	cli
	cmp eax,MULTIBOOT2_BOOTLOADER_MAGIC
	jnz  .error
	mov esp, kernel_stack	; setup a stack
	mov edi, eax	; save magic value into edi - will become rdi in x64 == first argument
	mov esi, ebx	; save multiboot header pointer in esi - will become rsi on x64 == second argument
.load_p4_table:
	; move page table address to cr3
	mov eax, (p4_table - VM_BASE)
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
	mov eax, (gdt - VM_BASE)
	lgdt [eax]        	; Load the new GDT pointer
	mov ax, DATA64  		; this is the offset into the GDT for our 64-bit data segment
	mov ds, ax        	; Load all data segment selectors
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	push esi
	push 0
	push edi
	push 0
.jmp_start64:
	jmp CODE64:start64   ; CODE is the offset into our GDT table for the 64-bit code segment: Far jump!
.loop:
	hlt				; should never get here
.error:
	xchg bx,bx
	jmp .loop

section .text

[BITS 64]
align 16
start64:
	and edi, edi	; 32-bit magic value is in edi. Make sure high order DWORD of rdi is cleared
	and esi, esi	; multiboot header pointer was put in esi. It's < 32 bits, make sure upper DWORD of rsi is cleared.
	mov r14, rdi	; save rdi & rsi while we call init_idt64_table & __libc_init_array
	mov r15, rsi
	call init_idt64_table
	call __libc_init_array
	mov rdi, r14
	mov rsi, r15
	jmp kmain64	; Jump to kmain64
	xchg bx,bx	
.loop:
	hlt				; should never get here
	jmp .loop	


section .multiboot.data
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
.end:

gdt:
	dw (entries.end - entries)
	dq entries
.end:

; Create initial identity mapped page table mapping the first 2MB of physical RAM
; the the first 2MB of virtual address space (i.e. virtual address == physical address )
align 0x1000
page_table:
p4_table:						; PML4E
	dq (p3_table + 3 - VM_BASE)			; pointer to PDPTE table with rw & present bits set
	TIMES 255 dq 0				; write 254 null entries to fill the table.
	dq (p3_table.next + 3 - VM_BASE)		; pointer to PDPTE table for high memory with rw & present bits set
	TIMES 253 dq 0				; write 254 null entries to fill the table.
	dq 387						; 1GB huge page maps the first 1GB of memory. 0xFFFFFFFF80000000 linear => 0x0000000000000000 physical
	dq (p4_table + 3 - VM_BASE)			; write final recursive entry. Constructing appropriate virtual addresses will give us access to the page tables
p3_table:						; PDPTE
	dq (p2_table + 3 - VM_BASE)			; pointer to PDE table with rw & present bits set.
	dq 0x0000000000000000
	dq 0x0000000000000000
	dq (p2_table + 0x03 - VM_BASE)
	TIMES 508 dq 0				; write 511 null entries to fill the table.
.next:
	; dq (p2_table.next + 3 - VM_BASE)		; pointer to PDE table with rw & present bits set.
	dq (p2_table + 3 - VM_BASE)		; pointer to PDE table with rw & present bits set.
	TIMES 511 dq 0				; write 511 null entries to fill the table.
p2_table:						; PDE 
	dq (p1_table + 3 - VM_BASE)			; pointer to the PTE table with rw & present bits set
	TIMES 511 dq 0				; write 511 null entries for a total of 512 entries
;.next:
;	dq (p1_table + 3 - VM_BASE)		; pointer to the High address PTE table with rw & present bits set
;	TIMES 511 dq 0				; write 511 null entries for a total of 512 entries
;.next2:
;	dq (p1_table + 3 - VM_BASE)			; pointer to the PTE table with rw & present bits set
;	TIMES 511 dq 0				; write 511 null entries for a total of 512 entries
p1_table:						; PTE 
	%assign p 0x03				; set the rw & present bits (0x03) 
	%rep 512					; write 511 of 512 entries to the PTE table
	dq p						; write page entry
	%assign p p+ 0x00001000		; increment page entry virtual ( and physical ) address by page size (4K)
	%endrep						; end of loop
page_table.end:
startup_data_end:
