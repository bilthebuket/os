bits 32

_start:


	mov [0xb8000], 'A'
	mov [0xb8001], 0fh

	cli
	hlt

	jmp spin


spin:
	jmp spin

error:
	cli
	hlt

