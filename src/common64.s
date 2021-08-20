section .text
[BITS 64]                       ; All instructions should be 32-bit.
[GLOBAL outb]
[GLOBAL outw]
[GLOBAL outd]
[GLOBAL inb]
[GLOBAL inw]
[GLOBAL ind]
[GLOBAL insl]

%include "macros.mac"

; void cpuid(CpuidInfo *info, uint32_t eax);
PROC cpuid
	test rdi, rdi
	jz .error
	mov eax, esi	; load eax with the passed in parameter
	cpuid
	mov dword [rdi], eax
	mov dword [rdi+4], ebx
	mov dword [rdi+8], ecx
	mov dword [rdi+12], edx
.exit:
	ret
.error:
	xor eax,eax
	jmp .exit
ENDP
PROC invalidate_tlb
	invlpg [rdi]
	ret
ENDP
PROC invalidate_all_tlbs
	mov rax, cr3
	mov cr3, rax
	ret
ENDP
PROC outb
	mov rdx, rdi
	mov rax, rsi
	out dx, al
	ret
ENDP
PROC outw
	mov rdx, rdi
	mov rax, rsi
	out dx, ax
	ret
ENDP
PROC outd
	mov rdx, rdi
	mov rax, rsi
	out dx, eax
	ret
ENDP
PROC outsb
	mov rdx, rdi
	mov rdi, rsi
	mov rcx, rdx
	rep outsb
	ret
ENDP
PROC outsw
	mov rdx, rdi
	mov rdi, rsi
	mov rcx, rdx
	rep outsw
	ret
ENDP
PROC inb
	mov rdx, rdi
	in al, dx
	ret
ENDP
PROC inw
	mov rdx, rdi
	in ax, dx
	ret
ENDP
PROC ind
	mov rdx, rdi
	in eax, dx
	ret
ENDP
; on entry rdi has port, rsi has address, rdx has size
; to execute, need rcx==size, rdi==address, rdx==port
PROC insb
	cld
	mov r10, rdx ; save size into r10
	mov rdx, rdi ; move port into rdx
	mov rcx, r10  ; move saved size into rcx
	mov rdi, rsi  ; move address into rdi
	rep insb	  ; execute the insb
	ret
ENDP
; on entry rdi has port, rsi has address, rdx has size
; to execute, need rcx==size, rdi==address, rdx==port
PROC insw
	cld
	mov r10, rdx ; save size into r10
	mov rdx, rdi ; move port into rdx
	mov rcx, r10  ; move saved size into rcx
	mov rdi, rsi  ; move address into rdi
	rep insw
	ret
ENDP
PROC batch_outb
	mov rcx, rdx
	test rcx,rcx
	jz .done
.loop:
	mov dx, WORD [rdi]
	mov al, BYTE [rsi]
	out dx, al
	add rdi, 2
	inc rsi
	dec rcx
	jnz .loop
.done:
	ret
ENDP
PROC batch_outb2
	test rsi,rsi
	jz .done
.loop:	
	mov edx, DWORD [rdi]
	mov al, dl
	shr edx, 16
	out dx, al
	add rdi, 4
	dec rsi
	jnz .loop
.done:
	ret
ENDP
PROC insl
	mov rcx, rdx
	mov rdx, rdi
	mov rdi, rsi
	rep insd
	ret
ENDP
PROC get_fault_addr64
	mov rax, cr2
	ret
ENDP
PROC halt
	cli
	hlt
	jmp halt
ENDP
PROC Cpuid
	xor rax,rax
	mov eax, [rdi]
	cpuid
	mov [rdi + 4], ebx
	mov [rdi + 8], ecx
	mov [rdi + 12], edx
	ret
ENDP
PROC dbgInterrupt
	mov bx, bx
	ret
ENDP