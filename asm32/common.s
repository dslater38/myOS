[BITS 32]                       ; All instructions should be 32-bit.

[GLOBAL outb]
[GLOBAL outw]
[GLOBAL outd]
[GLOBAL inb]
[GLOBAL inw]
[GLOBAL ind]
[GLOBAL idle_loop]
[GLOBAL set_page_directory]
[GLOBAL get_fault_addr]
[GLOBAL set_page64_directory]
[GLOBAL enable_paging_64]
[GLOBAL enable_paging_64_2]

[GLOBAL  p4_table]
[GLOBAL  p3_table]
[GLOBAL  p2_table]
[GLOBAL  p1_table]

[GLOBAL enable_interrupts]
	enable_interrupts:
		sti
		ret

[GLOBAL disable_interrupts]
	disable_interrupts:
		cli
		ret

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

outd:
	mov edx, [esp+4]
	mov eax, [esp+8]
	out dx, eax
	ret

inb:
	mov edx, [esp+4]
	in al, dx
	ret

inw:
	mov edx, [esp+4]
	in ax, dx
	ret

ind:
	mov edx, [esp+4]
	in eax, dx
	ret

idle_loop:
	hlt
	jmp idle_loop


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

set_page64_directory:
	;
	; move page table address to cr3
	;
	mov eax, [esp+4]
	mov	cr3, eax
	;
	; enable PAE
	;
	mov eax, cr4
	or eax, (1<<5)
	mov cr4, eax
	;
	; set the long mode bit
	;
	mov ecx, 0xC0000080
	rdmsr
	or eax, (1<<8)
	wrmsr
	;
	; enable 64-bit paging
	;
	mov eax, cr0	
	or eax, (1<<31)
	or eax, (1<<16)
	mov cr0, eax
	ret


enable_paging_64:
    ; Point the first entry of the level 4 page table to the first entry in the
    ; p3 table
    mov eax, p3_table
    or eax, 0b11 ;
    mov dword [p4_table + 0], eax

    ; Point the first entry of the level 3 page table to the first entry in the
    ; p2 table
    mov eax, p2_table
    or eax, 0b11
    mov dword [p3_table + 0], eax

    ; point each page table level two entry to a page
    mov ecx, 0         ; counter variable
.map_p2_table:
    mov eax, 0x200000  ; 2MiB
    mul ecx
    or eax, 0b10000011
    mov [p2_table + ecx * 8], eax

    inc ecx
    cmp ecx, 512
    jne .map_p2_table

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
	ret


enable_paging_64_2:
    ; Point the first entry of the level 4 page table (PML4E)to the first entry in the
    ; p3 table
    mov eax, p3_table
    or eax, 0b11 ;
    mov dword [p4_table + 0], eax

    ; Point the first entry of the level 3 page table (PDPTE)to the first entry in the
    ; p2 table
    mov eax, p2_table
    or eax, 0b11
    mov dword [p3_table + 0], eax
    
    
     ; point each page table level two entry to a page level 1 (PTE) entry
    mov ecx, 0         ; counter variable
    mov eax, p1_table
.map_p2_table:
    or eax, 0b00000011
    mov [p2_table + ecx * 8], eax

    inc ecx
    add eax, 0x1000
    cmp ecx, 512
    jne .map_p2_table
   
    
    ; point each page table level one (PTE) entry to a page
    mov ecx, 0         ; counter variable
.map_p1_table:
    mov eax, 0x1000  ; 4KB
    mul ecx
    or eax, 0b00000011
    mov [p1_table + ecx * 8], eax

    inc ecx
    cmp ecx, 512*2
    jne .map_p1_table

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
	ret
	


section .bss

align 4096

p4_table:			; PML4E
    resb 4096
p3_table:			; PDPTE
    resb 4096
p2_table:			; PDE with (PS=1 == 2mb pages, PS == 0 -> points to PTE)
    resb 4096
p1_table:
    resb 4096*512		; PTE 
