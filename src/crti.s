[BITS 64]                       ; All instructions should be 64-bit.
[global _init]
[global _fini]

;x86_64 crti.s ;
[section .init]
;.type _init, @function
_init:
	push rbp
	mov rbp, rsp
	; gcc will nicely put the contents of crtbegin.o's .init section here. ;

[section .fini]
;.type _fini, @function
_fini:
	push rbp
	mov rbp, rsp
	

	; gcc will nicely put the contents of crtbegin.o's .fini section here. */