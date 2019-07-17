[BITS 64]

%macro ISR_NOERRCODE 1  ; define a macro, taking one parameter
  [GLOBAL isr%1]        ; %1 accesses the first parameter.
  isr%1:
    cli
    push byte 0
    push byte %1
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
  [GLOBAL isr%1]
  isr%1:
    cli
    push qword %1
    jmp isr_common_stub
%endmacro

; This macro creates a stub for an IRQ - the first parameter is
; the IRQ number, the second is the ISR number it is remapped to.
%macro IRQ 2
  [GLOBAL irq%1]
  irq%1:
	cli
	push byte 0
	push byte %2
	jmp irq_common_stub
%endmacro


%macro COMMON_STUB 2
[EXTERN %2]

%1:
;	pusha                    ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax


	push qword r15
	push qword r14
	push qword r13
	push qword r12
	push qword r11
	push qword r10
	push qword r9
	push qword r8

	push qword rax
	push qword rcx
	push qword rdx
	push qword rbx
	push qword rsp
	push qword rbp
	push qword rsi
	push qword rdi
	
	mov ax, ds
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	push qword rax
	mov rdi, rsp

	; mov ax, ds               ; Lower 16-bits of eax = ds.
	; push rax                 ; save the data segment descriptor

	; mov ax, 0x10  ; load the kernel data segment descriptor
	; mov ds, ax
	; mov es, ax
	; mov fs, ax
	; mov gs, ax

	call %2

	pop rax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	pop rdi
	pop rsi
	pop rbp
	add rsp,8
	pop rbx
	pop rdx
	pop rcx
	pop rax
	
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15

	; pop rbx        ; reload the original data segment descriptor
	; mov ds, bx
	; mov es, bx
	; mov fs, bx
	; mov gs, bx

	pop qword r15
	pop qword r14
	pop qword r13
	pop qword r12
	pop qword r11
	pop qword r10
	pop qword r9
	pop qword r8
	pop qword rax
	pop qword rcx
	pop qword rdx
	pop qword rbx
	pop qword rbp
	add rsp, 8	; don't actually pop rsp
	pop qword rsi
	pop qword rdi
	
	add rsp, 8	; pop the error code pushed by the stub

	mov ds, rax
	pop qword rax
	mov es, rax
	pop qword rax

;	popa                     ; Pops edi,esi,ebp...
	add rsp,16	     ; Cleans up the pushed error code and pushed ISR number
	sti
	iretq           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
%endmacro


[GLOBAL enable_interrupts]
	enable_interrupts:
		sti
		ret


[GLOBAL disable_interrupts]
	disable_interrupts:
		cli
		ret


ISR_ERRCODE 0
ISR_ERRCODE 1
ISR_ERRCODE 2
ISR_ERRCODE 3
ISR_ERRCODE 4
ISR_ERRCODE 5
ISR_ERRCODE 6
ISR_ERRCODE 7
ISR_ERRCODE 8
ISR_ERRCODE 9
ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14
ISR_ERRCODE 15
ISR_ERRCODE 16
ISR_ERRCODE 17
ISR_ERRCODE 18
ISR_ERRCODE 19
ISR_ERRCODE 20
ISR_ERRCODE 21
ISR_ERRCODE 22
ISR_ERRCODE 23
ISR_ERRCODE 24
ISR_ERRCODE 25
ISR_ERRCODE 26
ISR_ERRCODE 27
ISR_ERRCODE 28
ISR_ERRCODE 29
ISR_ERRCODE 30
ISR_ERRCODE 31



IRQ   0,    32
IRQ   1,    33
IRQ   2,    34
IRQ   3,    35
IRQ   4,    36
IRQ   5,    37
IRQ   6,    38
IRQ   7,    39
IRQ   8,    40
IRQ   9,    41
IRQ   10,    42
IRQ   11,    43
IRQ   12,    44
IRQ   13,    45
IRQ   14,    46
IRQ   15,    47

COMMON_STUB isr_common_stub, isr64_handler
COMMON_STUB irq_common_stub, isr64_handler


; In isr.c
; [EXTERN isr_handler]

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
; isr_common_stub:
	; pusha                    ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

	; mov ax, ds               ; Lower 16-bits of eax = ds.
	; push eax                 ; save the data segment descriptor

	; mov ax, 0x10  ; load the kernel data segment descriptor
	; mov ds, ax
	; mov es, ax
	; mov fs, ax
	; mov gs, ax

	; call isr_handler

	; pop eax        ; reload the original data segment descriptor
	; mov ds, ax
	; mov es, ax
	; mov fs, ax
	; mov gs, ax

	; popa                     ; Pops edi,esi,ebp...
	; add esp, 8     ; Cleans up the pushed error code and pushed ISR number
	; sti
	; iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
   


; In isr.c
; [EXTERN irq_handler]

; This is our common IRQ stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame. 
; irq_common_stub:
	; pusha                    ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

	; mov ax, ds               ; Lower 16-bits of eax = ds.
	; push eax                 ; save the data segment descriptor

	; mov ax, 0x10  ; load the kernel data segment descriptor
	; mov ds, ax
	; mov es, ax
	; mov fs, ax
	; mov gs, ax

	; call irq_handler

	; pop ebx        ; reload the original data segment descriptor
	; mov ds, bx
	; mov es, bx
	; mov fs, bx
	; mov gs, bx

	; popa                     ; Pops edi,esi,ebp...
	; add esp, 8     ; Cleans up the pushed error code and pushed ISR number
	; sti
	; iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

