bits 32

section .text

_start:
	mov [0xb8000], 'A'
	mov [0xb8001], 0fh
spin:
	jmp spin
