bits 32
%include "shared_src/constants.asm"

section .text:

	global paging_enabled_huh

paging_enabled_huh:
	mov eax, cr0
	and eax, CR0_PAGING_BIT
	ret
