bits 32
%include "constants.asm"

section .text:

extern puts
extern putnum
extern prep_disk_isr

global request_disk_write
global request_disk_read
global disk_init
global ata_enable_interrupts

error:
	and eax, 0x000000FF
	push long 0x0F
	push eax
	push long 0x0
	push long 0x2
	call putnum

	mov dx, 0x1F1
	in al, dx
	and eax, 0x000000FF
	push long 0x0F
	push eax
	push long 0x0
	push long 0x1
	call putnum
	mov eax, 0
	ret 0xC

disk_read:
	mov ebx, [STAGE2_LOCATION]
	mov eax, [ebx + disk_request_info + 0x4]
	test eax, eax
	jz end
	sub eax, 1
	mov [ebx + disk_request_info + 0x4], eax
	mov edx, 0x1F0
	mov ecx, 0x100
	mov edi, [ebx + disk_request_info]
	rep insw
	mov [ebx + disk_request_info], edi
end:
	ret

disk_write:
	mov ebx, [STAGE2_LOCATION]
	mov eax, [ebx + disk_request_info + 0x4]
	test eax, eax
	jz end
	sub eax, 1
	mov [ebx + disk_request_info + 0x4], eax
	mov edx, 0x1F0
	mov ecx, 0x0
	mov esi, [ebx + disk_request_info]
	; using loop instead rep outsw because we need a delay in between writes to 0x1f0
write_loop:
	mov word ax, [esi]
	out dx, ax
	add esi, 2
	add ecx, 1
	cmp ecx, 0x100
	jb write_loop
	mov [ebx + disk_request_info], esi
	ret

request_disk_read:
	mov ebx, [STAGE2_LOCATION]
	add ebx, disk_read
	push ebx
	call prep_disk_isr
	mov cl, 0x20 ; read with retry
	call request_disk_generic

	mov eax, 1
	ret 8

request_disk_write:
	mov ebx, [STAGE2_LOCATION]
	add ebx, disk_write
	push ebx
	call prep_disk_isr
	mov cl, 0x30 ; write with retry
	call request_disk_generic

	mov edx, 0x1F7
	in al, dx
	test al, 0x8
	jz error
	call disk_write
	mov eax, 1
	ret 8

request_disk_generic:
	push ecx
	call wait_for_busy_disk
	cmp cx, DISK_TIMEOUT
	jae error
	pop ecx

	mov edx, 0x1F7
	in al, dx
	test al, 0x40
	jz error

	mov ebx, [STAGE2_LOCATION]

	mov eax, [esp + 0x10]
	mov [ebx + disk_request_info], eax
	mov eax, [esp + 0xC]
	mov [ebx + disk_request_info + 0x4], eax

	mov byte bl, [drive_bitmask + ebx]
	mov ebp, [esp + 0x8]
	mov eax, ebp
	mov edx, 0x01F6 ; send drive and bits 24-27 of lba
	shr eax, 24
	or al, bl
	out dx, al

	mov edx, 0x01F2 ; send number of sectors
	mov eax, [esp + 0xC]
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

	mov edx, 0x1F7 ; send command
	mov al, cl
	out dx, al

	call disk_wait_alternate

	in al, dx
	test al, 0x01
	jnz error
	
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
	mov dx, 0x1F7
	out dx, al
	ret

disk_init:
	mov ebx, [STAGE2_LOCATION]

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
	jae couldnt_find_disk
	
	mov cx, 0xF0
	call disk_identify
	call disk_wait
	in al, dx
	cmp al, 0
	jne found_disk
	
couldnt_find_disk:
	add ebx, disk_error_string
	push 0xF
	push ebx
	push 0x0
	push 0x0
	call puts
	cli
	hlt

found_disk:
	mov byte [ebx + drive_bitmask], cl

	call wait_for_busy_disk
	cmp cx, DISK_TIMEOUT
	jae couldnt_find_disk

	in al, dx
	test al, 0x08
	jz couldnt_find_disk

	mov cx, 0x0
	mov dx, 0x1F0
read_data:
	in ax, dx
	add cx, 0x1
	cmp cx, 0x100
	jb read_data

	ret

wait_for_busy_disk:
	mov cx, 0x0
	mov dx, 0x1F7
wait_loop:
	in al, dx
	test al, 0x80
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
	mov edx, 0x3F6
poll_alternate:
	in al, dx
	add cx, 0x1
	cmp cx, DISK_SELECT_WARMUP
	jb poll_alternate
	ret

ata_enable_interrupts:
	mov edx, 0x3F6
	mov al, 0x0
	out dx, al
	ret

section .data:

drive_bitmask:
	db 0x0

disk_request_info:
	dd 0x0 ; buffer address
	dd 0x0 ; number of sectors to read/write

disk_error_string: db "could not find a disk that exists", 0
