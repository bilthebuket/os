bits 32

_start:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

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

