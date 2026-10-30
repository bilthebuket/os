bits 32


section .text:

	global display_eip

	extern putnum

display_eip:
	pop eax
	push eax
	push long 0xf
	push eax
	push long 0
	push long 0
	push long 0
	call putnum
	ret
	
