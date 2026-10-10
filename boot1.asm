bits 16
org 0x7C00

_start:
	cli
	xor ax, ax
	mov ds, ax
	mov ss, ax
	mov sp, 0x7c00
	sti

	mov ax, [dap + 0x2]
	mov [stage2_sectors], ax
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
	add word [smap_offset], 0x14 ; again, i only care about the info in the first 20 bytes even if the bios can give me more
	add word [smap_size], 0x1
	cmp word [smap_size], 0x20
	jae exit_smap_loop
	test ebx, ebx
	jnz smap_loop

exit_smap_loop:

	mov eax, smap
	sub eax, 0x14
	mov ecx, eax
	shr eax, 0x4
	and ecx, 0x0F

	mov bx, [dap + 0x2]
	shl ebx, 0x9 ; bx now has the number of bytes we want to read from disk

find_avail_memory:
	add cx, 0x14
	mov es, ax
	mov di, cx
	cmp long [es:di + 0x10], 0x1 
	jne find_avail_memory
	cmp [es:di + 0x8], ebx
	jb find_avail_memory

	mov ebp, [es:di] ; making sure we arent writing over the stack where the stage 1 bootloader is loaded
	cmp ebp, smap_offset + 0x2

	add ebp, ebx
	jc find_avail_memory
	sub ebp, 0x1
	cmp ebp, 0x10FFEF
	ja find_avail_memory
	mov ebp, [es:di + 0x4]
	test ebp, ebp
	jnz find_avail_memory

	mov eax, [es:di]
	mov [stage2_location], eax 

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
	mov eax, [stage2_location]
	shr eax, 0x4
	mov [dap + 0x6], ax
	mov eax, [stage2_location]
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

	; gcc expects all args to be padded to 4 bytes each
	push word 0x0 ; padding
	push word [stage2_sectors]
	push long [stage2_location]
	push word 0x0 ; padding
	push word [smap_size]
	push long smap
	push long 0x0 ; push garbage address because gcc expects stage2 to be entered as a call and not a jmp

	cli
	lgdt [gdtr]
	mov eax, cr0
	or al, 1
	mov cr0, eax
	jmp 0x8:protected

protected:
bits 32
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	mov eax, [stage2_location]
	jmp eax
bits 16

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
	mov byte [es:di], ch
	mov byte [es:di + 1], 0fh

	mov ch, ah
	and ch, 0xF
	add ch, 'A'
	mov byte [es:di + 2], ch
	mov byte [es:di + 3], 0fh

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
	dw 0x2 ; read 2 sectors
	dw 0x0
	dw 0x0
	dq 0x1

smap_segment:
	dw 0x0

smap_offset:
	dw 0x0

smap_size:
	dw 0x0

stage2_location:
	dd 0x0

stage2_sectors:
	dw 0x0

smap:

; pad to 510 bytes, then add boot signature (total bin = 512 bytes)
times 510-($-$$) db 0
dw 0xAA55
