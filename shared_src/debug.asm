bits 32


section .text:

	global display_eip
	global error

	extern putnum

display_eip:
	pop eax
	push eax
	push long 0xf
	push eax
	push long 0
	push long 0
	call putnum
	ret

error:
	mov eax, [esp + 0x4]
	push long 0xf
	push eax
	push long 0
	push long 0
	call putnum
	ret 0x4
