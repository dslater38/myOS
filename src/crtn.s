[BITS 64]

[section .init]
label1:
	pop rbp
	ret

[section .fini]
label:
	pop rbp
	ret
