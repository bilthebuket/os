bits 32

section .text:
	global move_stack
	extern stage_two

move_stack:
	mov eax, [esp + 4]
	mov ecx, 0x7C00

copy_loop:
	sub ecx, 4
	sub eax, 4
	mov edx, [ecx]
	mov [eax], edx
	cmp ecx, esp
	ja copy_loop

	sub esp, ecx
	add eax, esp
	mov esp, eax

	add esp, 4 ; adding 4 to esp and jumping to stage two so gcc doesnt screw things up with ebp now that we have a new stack
	jmp stage_two
