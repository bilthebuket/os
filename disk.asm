bits 32

section .text:

global requst_disk_write
global request_disk_read

error:
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

	; for now we are only handling master drive
	mov ebx, [esp + 0x4]
	mov eax, ebx
	mov edx, 0x01F6 ; send drive and bits 24-27 of lba
	shr eax, 24
	or al, 11100000b
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
	out dx, cl
	
	in al, dx
	test al, 0x01
	jnz error

	mov eax, 1
	ret 8


