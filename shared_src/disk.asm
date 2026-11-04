bits 32
%include "shared_src/constants.asm"

section .text:

extern puts
extern putnum

global disk_init
global disk_read_poll
global disk_write_poll
global wait_for_busy_disk_alternate

disk_read_poll:
	push ebx
	push edi
	push ebp
	push esi

	lea ebx, [esp + 0x14]
	mov cl, ATA_READ
	call request_disk_generic
	test eax, 0xFFFFFFFF
	jnz poll_end

	cld
	mov edi, [esp + 0x20]
	mov ebx, [esp + 0x1C]

poll_read_loop:
	call wait_for_busy_disk

	mov edx, ATA_COMMAND_STATUS
	in al, dx
	test al, 0x8
	mov eax, ERROR_DATA_NOT_READY
	jz poll_end

	mov ecx, 0x100
	mov edx, ATA_DATA
	rep insw

	sub ebx, 1
	test ebx, ebx
	mov eax, 0
	jz poll_end
	jmp poll_read_loop

poll_end:
	pop esi
	pop ebp
	pop edi
	pop ebx
	ret 0x10

disk_write_poll:
	push ebx
	push edi
	push ebp
	push esi

	lea ebx, [esp + 0x14]
	mov cl, ATA_WRITE
	call request_disk_generic
	test eax, 0xFFFFFFFF
	jnz poll_end

	mov edi, [esp + 0x20]
	mov ebx, [esp + 0x1C]

poll_write_loop:
	call wait_for_busy_disk

	mov edx, ATA_COMMAND_STATUS
	in al, dx
	test al, 0x8
	mov eax, ERROR_DATA_NOT_READY
	jz poll_end

	mov ecx, 0x100
	mov edx, ATA_DATA
write_data_loop:
	mov ax, [edi]
	out dx, ax
	sub ecx, 1
	test ecx, ecx
	jnz write_data_loop

	sub ebx, 1
	test ebx, ebx
	mov eax, 0
	jz poll_end
	jmp poll_write_loop

error:
	add esp, ebx
	ret

request_disk_generic: ; ebx = pointer to the 4 argmuents, ecx = read or write
	mov ebp, ebx
	push ecx
	call wait_for_busy_disk
	cmp cx, DISK_TIMEOUT
	pop ecx
	mov eax, ERROR_DISK_TIMEOUT
	mov ebx, 0
	jae error

	mov edx, ATA_COMMAND_STATUS
	in al, dx
	test al, ATA_STATUS_READY
	mov eax, ERROR_NOT_READY
	jz error

	push cx
	mov ebx, ebp

	mov ecx, [ebx]
	mov ebp, [ebx + 0x4]
	mov eax, ebp
	mov edx, 0x01F6 ; send drive and bits 24-27 of lba
	shr eax, 24
	or al, cl
	out dx, al

	mov edx, 0x01F2 ; send number of sectors
	mov eax, [ebx + 0x8]
	out dx, al

	mov edx, 0x1F3 ; send bits 0-7 of lba
	mov eax, ebp
	out dx, al

	mov edx, 0x1F4 ; send bits 8-15 of lba
	mov eax, ebp
	shr eax, 0x8
	out dx, al

	mov edx, 0x1F5 ; send bits 16-23 of lba
	mov eax, ebp
	shr eax, 0x10
	out dx, al

	pop cx

	mov edx, ATA_COMMAND_STATUS ; send command
	mov al, cl
	out dx, al

	call disk_wait_alternate

	in al, dx
	test al, ATA_STATUS_ERROR
	mov eax, ERROR_DISK_ERROR
	mov ebx, 0
	jnz error
	
	mov eax, 0
	ret

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
	mov dx, ATA_COMMAND_STATUS
	out dx, al
	ret

disk_init:
	push ebx
	push esi
	push edi
	push ebp

	mov dx, 0x01F6
	mov al, 0xE0 ; try master drive
	out dx, al

	call disk_wait
	call wait_for_busy_disk
	cmp cx, DISK_TIMEOUT
	jae continue

	call disk_identify
	call disk_wait
	in al, dx
	cmp al, 0
	je continue

	mov cx, 0xE0
	jmp found_disk

continue:
	mov dx, 0x01F6
	mov al, 0xF0 ; try slave drive
	out dx, al

	call disk_wait
	call wait_for_busy_disk
	cmp cx, DISK_TIMEOUT
	mov eax, ERROR_COULDNT_FIND_DISK
	mov ebx, 0x10
	jae error
	
	mov cx, 0xF0
	call disk_identify
	call disk_wait
	in al, dx
	cmp al, 0
	mov eax, ERROR_COULDNT_FIND_DISK
	mov ebx, 0x10
	je error
	
found_disk:
	push cx

	call wait_for_busy_disk
	cmp cx, DISK_TIMEOUT
	mov eax, ERROR_COULDNT_FIND_DISK
	mov ebx, 0x12
	jae error

	in al, dx
	test al, ATA_STATUS_DRQ
	mov eax, ERROR_COULDNT_FIND_DISK
	mov ebx, 0x12
	jz error

	mov cx, 0x0
	mov dx, ATA_DATA
read_data:
	in ax, dx
	add cx, 0x1
	cmp cx, 0x100
	jb read_data

	pop cx
	mov eax, ecx
	pop ebp
	pop edi
	pop esi
	pop ebx
	ret

wait_for_busy_disk_alternate:
	mov cx, 0x0
	mov dx, ATA_CONTROL_ALT_STATUS
	jmp wait_loop

wait_for_busy_disk:
	mov cx, 0x0
	mov dx, ATA_COMMAND_STATUS
wait_loop:
	in al, dx
	test al, ATA_STATUS_BUSY
	jz not_busy
	add cx, 0x1
	cmp cx, DISK_TIMEOUT
	jb wait_loop

not_busy:
	ret

disk_wait:
	mov cx, 0x0
	mov dx, 0x1F7
poll:
	in al, dx
	add cx, 0x1
	cmp cx, DISK_SELECT_WARMUP
	jb poll
	ret

disk_wait_alternate:
	mov cx, 0x0
	mov edx, ATA_CONTROL_ALT_STATUS
poll_alternate:
	in al, dx
	add cx, 0x1
	cmp cx, DISK_SELECT_WARMUP
	jb poll_alternate
	ret
