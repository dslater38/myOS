[BITS 32]

[GLOBAL start]
[GLOBAL gdt_flush]
[GLOBAL mboot_header]
[EXTERN init_gdt]
[EXTERN kmain64]
[EXTERN init_stack32];

;
; Multiboot header definitions
;


%define MULTIBOOT2_BOOTLOADER_MAGIC       0x36d76289
%define MULTIBOOT2_HEADER_MAGIC               0xe85250d6

%define MULTIBOOT_TAG_ALIGN                  8
%define MULTIBOOT_TAG_TYPE_END               0
%define MULTIBOOT_TAG_TYPE_CMDLINE           1
%define MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME  2
%define MULTIBOOT_TAG_TYPE_MODULE            3
%define MULTIBOOT_TAG_TYPE_BASIC_MEMINFO     4
%define MULTIBOOT_TAG_TYPE_BOOTDEV           5
%define MULTIBOOT_TAG_TYPE_MMAP              6
%define MULTIBOOT_TAG_TYPE_VBE               7
%define MULTIBOOT_TAG_TYPE_FRAMEBUFFER       8
%define MULTIBOOT_TAG_TYPE_ELF_SECTIONS      9
%define MULTIBOOT_TAG_TYPE_APM               10
%define MULTIBOOT_TAG_TYPE_EFI32             11
%define MULTIBOOT_TAG_TYPE_EFI64             12
%define MULTIBOOT_TAG_TYPE_SMBIOS            13
%define MULTIBOOT_TAG_TYPE_ACPI_OLD          14
%define MULTIBOOT_TAG_TYPE_ACPI_NEW          15
%define MULTIBOOT_TAG_TYPE_NETWORK           16
%define MULTIBOOT_TAG_TYPE_EFI_MMAP          17
%define MULTIBOOT_TAG_TYPE_EFI_BS            18
%define MULTIBOOT_TAG_TYPE_EFI32_IH          19
%define MULTIBOOT_TAG_TYPE_EFI64_IH          20
%define MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR    21

%define MULTIBOOT_HEADER_TAG_END  dw 0
%define MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST  dw 1
%define MULTIBOOT_HEADER_TAG_ADDRESS  dw 2
%define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS  dw 3
%define MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS  dw 4
%define MULTIBOOT_HEADER_TAG_FRAMEBUFFER  dw 5
%define MULTIBOOT_HEADER_TAG_MODULE_ALIGN  dw 6
%define MULTIBOOT_HEADER_TAG_EFI_BS        dw 7
%define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI32  dw 8
%define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI64  dw 9
%define MULTIBOOT_HEADER_TAG_RELOCATABLE  dw 10

%define MULTIBOOT_ARCHITECTURE_I386  0
%define MULTIBOOT_ARCHITECTURE_MIPS32  4
%define MULTIBOOT_HEADER_TAG_OPTIONAL dw 1

%define MULTIBOOT_LOAD_PREFERENCE_NONE 0
%define MULTIBOOT_LOAD_PREFERENCE_LOW 1
%define MULTIBOOT_LOAD_PREFERENCE_HIGH 2

%define MULTIBOOT_CONSOLE_FLAGS_CONSOLE_REQUIRED dd 1
%define MULTIBOOT_CONSOLE_FLAGS_EGA_TEXT_SUPPORTED dd 2


;
;	GDT macro definitions
;

%define GDT_FLAG_ACCESSED 		(1<<40)
%define GDT_READ_WRITE 			(1<<41)		; read access for code segments, write access for data segments

%define GDT_CONFORMING 			(1<<42)		; for code segements only
%define GDT_DIRECTION_DOWN 		(1<<42)		; for data segments only
%define GDT_DIRECTION_UP	 		(0<<42)		; for data segments only

%define GDT_CODE_SEGMENT 			(1<<43)		; for code segments only
%define GDT_DATA_SEGMENT 			(0<<43)		; for data segments only

%define GDT_DESCRIPTOR_CODE_DATA	(1<<44)		; always set for code & data segments. Cleared for system segments.
%define GDT_DESCRIPTOR_SYSTEM 		(0<<44)		; system segment

%define GDT_RING0					(0<<45)
%define GDT_RING1					(1<<45)
%define GDT_RING2					(2<<45)
%define GDT_RING3					(3<<45)

%define GDT_PRESENT 				(1<<47)

; %define GDT_LONG_MODE			(1<<53)		; enable long (64-bit) mode	(long mode bit) - use GDT_64_BIT_OPERAND macro for this
%define GDT_32_BIT_OPERAND 		(1<<54)		; 32-bit protected code	bit 54 set		bit 53 cleared
%define GDT_16_BIT_OPERAND 		(0<<54)		; 16-bit protected code	bit 54 cleared	bit 53 cleared
%define GDT_64_BIT_OPERAND 		(1<<53)		; 64-bit protected code 	bit 54 cleared	bit 53 set 

%define GDT_GRAN_4K				(1<<55)		; Granularity 1 byre or 4k
%define GDT_GRAN_1_BYTE			(0<<55)		; Granularity 1 byre or 4k

; use as GDT base, limit, flags
%MACRO GDT 3
	dq (%2 & 0xFFFF)|( (%1 & 0xFFFFFF)<<16) |( (%2 & 0x0F0000) << 32) | ((%1 & 0xFF000000) << 32) | (%3)
%ENDMACRO

; on entry, ebx holds the multi-boot header pointer
; eax holds the magic value

start:
	cli
	cmp eax,MULTIBOOT2_BOOTLOADER_MAGIC
	jnz  .error
	mov esp, init_stack32	; setup a stack
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


