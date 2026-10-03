bits 16
org 0x7C00

_start:
	cli
	xor ax, ax
	mov ds, ax
	mov ss, ax
	mov sp, 0x7c00
	sti

	mov [boot_drive], dl
	mov ah, dl

	mov ax, 0x3
	int 0x10
	jc error

	mov eax, smap
	shr eax, 0x4
	mov [smap_segment], ax
	mov eax, smap
	and eax, 0x0F
	mov [smap_offset], ax
	xor ebx, ebx

smap_loop:
	mov es, [smap_segment]
	mov di, [smap_offset]
	mov ax, 0xE820
	mov cx, 0x14 ; size = 20 bytes, for now i dont need any of the other information so i wont request it
	mov edx, 0x534D4150
	int 0x15
	jc error 
	cmp eax, 0x534D4150
	jne error
	add [smap_offset], 0x14 ; again, i only care about the info in the first 20 bytes even if the bios can give me more
	test ebx, ebx
	jnz smap_loop

	mov eax, smap
	sub eax, 0x14
	mov ecx, eax
	shr eax, 0x4
	and ecx, 0x0F

	mov bx, [dap + 0x2]
	shl ebx, 0x9 ; bx now has the number of bytes we want to read from disk

	mov edx, 0xFFFF ; we're gonna perform a far jmp to a 16 bit seg:offset which is where our code will be loaded from the disk read
	; however, because our gdt puts the segment starting at 0x0, the maximum address we can reach with a descriptor segment + offset is 0xFFFF

	; this relies on the first piece of available memory we find not colliding with the bootloader stuff loaded at 0x7c00
find_avail_memory:
	add cx, 0x14
	mov es, ax
	mov di, cx
	cmp [es:di + 0x10], 0x1
	jne find_avail_memory
	cmp [es:di + 0x8], ebx
	jb find_avail_memory
	mov ebp, [es:di]
	add ebp, ebx
	jc find_avail_memory
	sub ebp, 0x1
	cmp ebp, edx
	ja find_avail_memory
	mov ebp, [es:di + 0x4]
	test ebp, ebp
	jnz find_avail_memory

	mov dl, [boot_drive]
	mov ah, 0x41
	mov bx, 0x55AA
	int 0x13
	jc error

	cmp bx, 0xAA55
	jne error

	test cx, 1
	jz error

	; read stage 2 into memory
	mov eax, [es:di]
	mov ebx, [es:di] ; storing this address so we can jmp to it after entering protected mode
	shr eax, 0x4
	mov [dap + 0x6], ax
	mov eax, [es:di]
	and eax, 0x0F
	mov [dap + 0x4], ax
	mov ax, dap ; we know dap is somewhere around 0x7c00 so it will be less than 0xFFFF
	mov si, ax
	xor ax, ax
	mov ds, ax
	mov ah, 0x42
	mov dl, [boot_drive]
	int 0x13
	jc error

	mov ecx, smap
	mov [stage_two], bx

	cli
	lgdt [gdtr]
	mov eax, cr0
	or al, 1
	mov cr0, eax
	jmp far [stage_two]

error:
	cli
	hlt

debug:
	mov bx, 0xb800
	mov es, bx
	xor bx, bx
	mov di, bx

	mov ch, ah
	shr ch, 0x4
	add ch, 'A'
	mov [es:di], ch
	mov [es:di + 1], 0fh

	mov ch, ah
	and ch, 0xF
	add ch, 'A'
	mov [es:di + 2], ch
	mov [es:di + 3], 0fh

	cli
	hlt

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

boot_drive:
	db 0x0

align 4
dap:
	db 0x10
	db 0x00
	dw 0x1 ; read 1 sector
	dw 0x0
	dw 0x0
	dq 0x1

stage_two:
	dw 0
	dw 0x8

smap_segment:
	dw smap + 0x280

smap_offset:
	dw smap + 0x282

smap:

; pad to 510 bytes, then add boot signature (total bin = 512 bytes)
times 510-($-$$) db 0
dw 0xAA55
