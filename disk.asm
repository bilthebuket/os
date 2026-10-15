bits 32
%include "constants.asm"

section .text:

extern puts
extern putnum

global request_disk_write
global request_disk_read
global disk_init

error:
	push long 0x0F
	push eax
	push long 0x0
	push long 0x1
	call putnum
	mov eax, 0
	ret 8

request_disk_read:
	mov cl, 0x20 ; read with retry
	jmp request_disk_generic

request_disk_write:
	mov cl, 0x30 ; write with retry
	jmp request_disk_generic

request_disk_generic:
	mov edx, 0x1F7
	in al, dx
	cmp al, 0x40
	jne error

	mov ecx, [STAGE2_LOCATION]
	mov byte cl, [drive_bitmask + ecx]
	mov ebx, [esp + 0x4]
	mov eax, ebx
	mov edx, 0x01F6 ; send drive and bits 24-27 of lba
	shr eax, 24
	or al, cl
	out dx, al

	mov edx, 0x01F2 ; send number of sectors
	mov eax, [esp + 0x8]
	out dx, al

	mov edx, 0x1F3 ; send bits 0-7 of lba
	mov eax, ebx
	out dx, al

	mov edx, 0x1F4 ; send bits 8-15 of lba
	mov eax, ebx
	shr eax, 0x8
	out dx, al

	mov edx, 0x1F5 ; send bits 16-23 of lba
	mov eax, ebx
	shr eax, 0x10
	out dx, al

	mov edx, 0x1F7 ; send command
	mov al, cl
	out dx, al
	
	in al, dx
	test al, 0x01
	jnz error

	mov eax, 1
	ret 8

disk_identify:
	mov al, 0
	mov dx, 0x1F2
	out dx, al
	mov dx, 0x1F3
	out dx, al
	mov dx, 0x1F4
	out dx, al
	mov dx, 0x1F5
	out dx, al
	mov al, 0xEC
	mov dx, 0x1F7
	out dx, al
	in al, dx
	ret

disk_init:
	mov ebx, [STAGE2_LOCATION]

	mov dx, 0x01F6
	mov al, 0xA0 ; try master drive
	out dx, al

	call disk_identify
	cmp al, 0
	je continue

	add ebx, drive_bitmask
	mov byte [ebx], 0xA0
	ret

continue:
	mov dx, 0x01F6
	mov al, 0xB0 ; try slave drive
	out dx, al

	call disk_identify
	cmp al, 0
	jne found_disk
	
	add ebx, error_string
	push 0xF
	push ebx
	push 0x0
	push 0x0
	call puts
	cli
	hlt

found_disk:
	add ebx, drive_bitmask
	mov byte [ebx], 0xB0
	ret

section .data:

drive_bitmask:
	db 0x0

error_string: db "could not find a disk that exists", 0
