section .text
[BITS 64]                       ; All instructions should be 32-bit.
; [GLOBAL outb]
; [GLOBAL outw]
; [GLOBAL outd]
; [GLOBAL inb]
; [GLOBAL inw]
; [GLOBAL ind]
; [GLOBAL insl]

%include "macros.mac"

; void cpuid(CpuidInfo *info, uint32_t eax);
PROC cpuid
	ENTER
	test rdi, rdi
	jz .error
	mov eax, esi	; load eax with the passed in parameter
	cpuid
	mov dword [rdi], eax
	mov dword [rdi+4], ebx
	mov dword [rdi+8], ecx
	mov dword [rdi+12], edx
.exit:
	LEAVE
	ret
.error:
	xor eax,eax
	jmp .exit
ENDP
PROC invalidate_tlb
	ENTER
	mov rax, cr3
	mov cr3, rax
;	invlpg [rdi]
	LEAVE
	ret
ENDP
PROC invalidate_all_tlbs
	ENTER
	mov rax, cr3
	mov cr3, rax
	LEAVE
	ret
ENDP
PROC pml4_addr
	ENTER
	mov rax, cr3
	LEAVE
	ret
ENDP
PROC outb
	ENTER
	mov rdx, rdi
	mov rax, rsi
	out dx, al
	LEAVE
	ret
ENDP
PROC outw
	ENTER
	mov rdx, rdi
	mov rax, rsi
	out dx, ax
	LEAVE
	ret
ENDP
PROC outd
	ENTER
	mov rdx, rdi
	mov rax, rsi
	out dx, eax
	LEAVE
	ret
ENDP
PROC outsb
	ENTER
	mov rdx, rdi
	mov rdi, rsi
	mov rcx, rdx
	rep outsb
	LEAVE
	ret
ENDP
PROC outsw
	ENTER
	mov rdx, rdi
	mov rdi, rsi
	mov rcx, rdx
	rep outsw
	LEAVE
	ret
ENDP
PROC inb
	ENTER
	mov rdx, rdi
	in al, dx
	LEAVE
	ret
ENDP
PROC inw
	ENTER
	mov rdx, rdi
	in ax, dx
	LEAVE
	ret
ENDP
PROC ind
	ENTER
	mov rdx, rdi
	in eax, dx
	LEAVE
	ret
ENDP
; on entry rdi has port, rsi has address, rdx has size
; to execute, need rcx==size, rdi==address, rdx==port
PROC insb
	ENTER
	cld
	mov r10, rdx ; save size into r10
	mov rdx, rdi ; move port into rdx
	mov rcx, r10  ; move saved size into rcx
	mov rdi, rsi  ; move address into rdi
	rep insb	  ; execute the insb
	LEAVE
	ret
ENDP
; on entry rdi has port, rsi has address, rdx has size
; to execute, need rcx==size, rdi==address, rdx==port
PROC insw
	ENTER
	cld
	mov r10, rdx ; save size into r10
	mov rdx, rdi ; move port into rdx
	mov rcx, r10  ; move saved size into rcx
	mov rdi, rsi  ; move address into rdi
	rep insw
	LEAVE
	ret
ENDP
PROC batch_outb
	ENTER
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
	LEAVE
	ret
ENDP
PROC batch_outb2
	ENTER
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
	LEAVE
	ret
ENDP
PROC insl
	ENTER
	mov rcx, rdx
	mov rdx, rdi
	mov rdi, rsi
	rep insd
	LEAVE
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
	ENTER
	xor rax,rax
	mov eax, [rdi]
	cpuid
	mov [rdi + 4], ebx
	mov [rdi + 8], ecx
	mov [rdi + 12], edx
	LEAVE
	ret
ENDP
PROC dbgInterrupt
	mov bx, bx
	ret
ENDP
PROC acquireLock
	ENTER
	xor rax, rax
	test rdi, rdi
	jz .exit
	lock bts dword [rdi], 0
	jc .spin
	inc rax
.exit
	LEAVE
	ret
.spin
	pause
	bt dword [rdi],0
	jnz .spin
	jmp acquireLock
ENDP
PROC releaseLock
	ENTER
	xor rax, rax
	test rdi, rdi
	jz .exit
	mov dword [rdi], 0
	inc rax
.exit:	
	LEAVE
	ret
ENDP
