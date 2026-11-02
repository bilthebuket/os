bits 32
%include "shared_src/disk.asm"
%include "shared_src/constants.asm"

section .text:

extern prep_disk_isr

global ata_enable_interrupts
global disk_request_read
global disk_request_write

ata_enable_interrupts:
	mov edx, ATA_CONTROL_ALT_STATUS
	mov al, 0x0
	out dx, al
	ret

disk_read:
	mov long ebx, [num_sectors]
	test ebx, ebx
	jz end
	mov long edi, [buf]
	mov ecx, 0x100
	mov edx, ATA_DATA
	rep insw
	jmp update_info

disk_write:
	mov long ebx, [num_sectors]
	test ebx, ebx
	jz end
	mov long esi, [buf]
	mov ecx, 0x100
	mov edx, ATA_DATA
	cld

; using a loop because we need a small delay inbetween writes
write_loop:
	mov word ax, [esi]
	out dx, ax
	add esi, 2
	sub ecx, 1
	test ecx, ecx
	jnz write_loop

update_info:
	mov long [buf], esi
	sub ebx, 1
	mov long [num_sectors], ebx
end:
	ret

disk_request_read:
	push ebx
	push edi
	push ebp
	push esi

	lea ebx, [esp + 0x14]
	mov cl, ATA_READ
	call request_disk_generic
	test eax, 0xFFFFFFFF
	jnz request_end

	mov long eax, [esp + 0x20]
	mov long [buf], eax
	mov long eax, [esp + 0x1C]
	mov long [num_sectors], eax
	mov long eax, [esp + 0x14]
	mov byte [drive_mask], al

	push disk_read
	call prep_disk_isr

	mov eax, 0

request_end:
	pop esi
	pop ebp
	pop edi
	pop ebx
	ret 0x10

disk_request_write:
	push ebx
	push edi
	push ebp
	push esi

	lea ebx, [esp + 0x14]
	mov cl, ATA_WRITE
	call request_disk_generic
	test eax, 0xFFFFFFFF
	jnz request_end
	
	mov edx, ATA_CONTROL_ALT_STATUS
	in al, dx
	test al, ATA_STATUS_DRQ
	mov eax, ERROR_DATA_NOT_READY
	jz request_end

	mov long eax, [esp + 0x20]
	mov long [buf], eax
	mov long [num_sectors], 1
	mov long eax, [esp + 0x14]
	mov byte [drive_mask], al

	call disk_write
	mov long eax, [esp + 0x1C]
	sub eax, 1
	mov long [num_sectors], eax

	push disk_write
	call prep_disk_isr
	
	mov eax, 0
	jmp request_end

section .data:

buf:
	dd 0x0
num_sectors:
	dd 0x0
drive_mask:
	db 0x0
