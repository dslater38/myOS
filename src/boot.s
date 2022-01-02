section .text
[BITS 32]

[GLOBAL start]
; [GLOBAL gdt_flush]
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

%define CODE64 (entries.code-entries)
%define DATA64 (entries.data-entries)
%define USERCODE64 (entries.user_code-entries)
%define USERDATA64 (entries.user_data-entries)
%define USERCODE32 (entries.user_code_32-entries)
%define USERDATA32 (entries.user_data_32-entries)
%define USERCODE16 (entries.user_code_16-entries)
%define USERDATA16 (entries.user_data_16-entries)


section .multiboot.text

; on entry, ebx holds the multi-boot header pointer
; eax holds the magic value
align 16
; start:
PROC start
	cli
	cmp eax,MULTIBOOT2_BOOTLOADER_MAGIC
	jnz  .error
;	mov esp, kernel_stack	; setup a stack
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
	mov eax, (gdt - VM_BASE); We're running 32-bit code. Need to initially load the physical address of gdt
	lgdt [eax]        		; Load the new GDT pointer. Currently, we have the first 4 MB identity mapped
	mov ax, DATA64  		; this is the offset into the GDT for our 64-bit data segment
	mov ds, ax        		; Load all data segment selectors
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
.jmp_start64:
	jmp CODE64:tramp64   ; CODE is the offset into our GDT table for the 64-bit code segment: Far jump!
.loop:
	hlt				; should never get here
.error:
	xchg bx,bx
	jmp .loop
	pop ebp			; undo the 2 pushes at the start
	pop ebp			; first pop is high-order DWORD == 0 - discard
ENDP

[BITS 64]
align 16
PROC tramp64
	mov rsp, kernel_stack
	mov rbp, tramp64
	push rbp
	mov rbp, rsp
	push rsi
	push rdi
	mov r10, start64
	jmp r10
ENDP

section .text

[BITS 64]
align 16
PROC start64
	mov rax, gdt				; Now that we're in long mode, reload our GDT with the full
	lgdt [rax]      			; 64-bit higher-half virtual address
	mov rax, p4_table			; Now that the GDT is reloaded, we no longer need the identity mapping
	mov qword [rax], 0x00000000	; so, zero out the first entry of the PML4# table
	mov rax, cr3				; and reload the page tables.
	mov cr3, rax				; At this point, only virtual addresses above VM_BASE (0xFFFF800000000000) should be valid.
	push start					; setup fake return address - allows debugger to walk up the stack
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
ENDP

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
p4_table:								; PML4E
	dq (p3_table + 3 - VM_BASE)			; level 3 entry for identity mapping. This aliases the higher-half kernel's physical memory
	TIMES 255 dq 0						; write 254 null entries to fill the table.
	dq (p3_table + 3 - VM_BASE)			; level 3 entry for the higher-half kernel. We map 4 MB at VM_BASE (0xFFFF800000000000)
	TIMES 254 dq 0						; write 254 null entries to fill the table.
	dq (p4_table + 3 - VM_BASE)			; write final recursive entry. Constructing appropriate virtual addresses will give us access to the page tables
p3_table:								; PDPTE
	dq (p2_table + 3 - VM_BASE)			; level 2 entry
	TIMES 511 dq 0						; write 511 null entries to fill the table.
p2_table:								; PDE 
	dq (p1_table + 3 - VM_BASE)			; 2 level 1 entries to map 4MB of physical memory
	dq (p1_table.next + 3 - VM_BASE)	; 
	TIMES 510 dq 0						; write 510 null entries for a total of 512 entries
p1_table:								; PTE 2 PTE's to map the first 4 MB of physical memory to VM_BASE (0xFFFF800000000000)
	%assign p 0x03						; set the rw & present bits (0x03) 
	%rep 512							; write 511 of 512 entries to the PTE table
	dq p								; write page entry
	%assign p p + 0x00001000			; increment page entry virtual ( and physical ) address by page size (4K)
	%endrep								; end of loop
p1_table.next:
	%assign p 0x00200003				; set the rw & present bits (0x03) 
	%rep 512							; write 511 of 512 entries to the PTE table
	dq p								; write page entry
	%assign p p + 0x00001000			; increment page entry virtual ( and physical ) address by page size (4K)
	%endrep								; end of loop
page_table.end:

startup_data_end:
