bits 32

section .text

_start:
	mov ax, 0x3
	int 0x10
	jc error

	mov [0xb8000], 'A'
	mov [0xb8001], 0fh
spin:
	jmp spin

error:
	cli
	hlt
