bits 16
org 0x7C00

section .bss

boot_drive resb 1
smap resb 0x280 ; reserve enough for 32 20-byte entries
smap_segment resb 2
smap_offset resb 2

section .data
align 4
dap:
	db 0x10
	db 0x00
	dw 0x1 ; read 1 sector
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

stage_two:
	dw 0x8
	dd 0

section .text
	global _start

_start:
	xor ax, ax
	mov ds, ax
	mov eax, smap
	shr eax, 0x4
	mov [smap_segment], ax
	mov eax, smap
	and eax, 0x0F
	mov [smap_offset], ax
	xor ebx, ebx

smap_loop:
	xor ax, ax
	mov es, [smap_segment]
	mov ax, 0xE820
	mov cx, 0x14 ; size = 20 bytes, for now i dont need any of the other information so i wont request it
	mov edx, 0x534D4150
	mov di, [smap_offset]
	int 0x15
	jc error 
	cmp eax, 0x534D4150
	jne error
	add [smap_offset], 0x14 ; again, i only care about the info in the first 20 bytes even if the bios can give me more
	test ebx, ebx
	jnz smap_loop

	mov eax, smap
	sub eax, 0x14
	shr eax, 0x4
	mov [smap_segment], ax
	mov eax, smap
	and eax, 0x0F
	mov [smap_offset], ax

	mov bx, [dap + 0x2]
	shl ebx, 0x9 ; bx now has the number of bytes we want to read from disk

find_avail_memory:
	add [smap_offset], 0x14
	mov ax, [smap_segment]
	mov es, ax
	mov ax, [smap_offset]
	mov di, ax
	cmp [es:di + 0x10], 0x1
	jne find_avail_memory
	cmp [es:di + 0x8], ebx
	jb find_avail_memory

	; read stage 2 into memory
	mov eax, [es:di]
	shr eax, 0x4
	mov ebx, eax
	mov [dap + 0x6], ax
	mov eax, [es:di]
	and eax, 0x0F
	add ebx, eax
	mov [dap + 0x4], ax
	mov [boot_drive], dl
	mov ah, 0x42
	mov eax, dap
	shr eax, 0x4
	mov ds, ax
	mov eax, dap
	and eax, 0x0F
	mov si, ax
	int 0x13
	jc error

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
	mov [stage_two + 0x2], ebx

	jmp far [stage_two]

error:
	cli
	hlt

; pad to 510 bytes, then add boot signature (total bin = 512 bytes)
times 510-($-$$) db 0
dw 0xAA55
