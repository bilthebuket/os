section .bss

boot_drive resb 1
smap resb 0x294 ; reserve enough for 32 20-byte entries
smap_ptr resb 4

section .data
dap:
	db 0x10
	db 0x00
	dw 0xA ; load 10 sectors
	dw 0x0
	dw 0x0
	dq 0x1

gdt:
	dq 0x0

	dw 0xFFFF
	dw 0x0
	db 0x0
	db 0x9A
	db 0xCF
	db 0x0

	dw 0xFFFF
	dw 0x0
	db 0x0
	db 0x92
	db 0xCF
	db 0x0
gdt_end:

gdtr:
	dw gdt_end - gdt - 1
	dd gdt

section .text
	global _start

_start:
	mov [iterator], 0
	mov [smap_ptr], smap
	mov ax, 0xE820
	mov bx, 0x0
	mov cx, 0x14 ; size = 20 bytes
	mov es, 0x0

smap_loop:
	mov di, [smap_ptr]
	int 0x15
	jc error
	add [smap_ptr], cx
	cmp bx, 0x0
	jne smap_loop

	mov [smap_ptr], smap
	sub [smap_ptr], cx
	mov ebx, [dap + 0x2]
	shl ebx, 0x9 ; ebx now has the number of bytes we want to read from disk

find_avail_memory:
	add [smap_ptr], cx
	cmp [smap_ptr + 0x10], 0x1
	jne find_avail_memory
	cmp [smap_ptr + 0x8], ebx
	jb find_avail_memory

	; read stage 2 into memory
	mov ebx, [smap_ptr]
	mov ecx, ebx
	shr ecx, 0x4
	mov [dap + 0x4], bx
	mov [dap + 0x6], cx
	mov ah, 0x42
	mov si, dap
	mov [boot_drive], dl
	int 0x13
	jc error
	mov dl, [boot_drive]

	cli
	lgdt [gdtr]
	mov eax, cr0
	or al, 1
	mov cr0, eax

	mov ecx, smap
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x8:ebx

error:
	cli
	hlt
