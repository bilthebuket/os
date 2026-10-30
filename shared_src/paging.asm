bits 32

section .text:

	global paging_enabled_huh

paging_enabled_huh:
	mov eax, cr0
	and eax, 0x80000000
	ret
