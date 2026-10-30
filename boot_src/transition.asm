bits 32

lgdt [esp + 4]
mov eax, [esp + 8]

mov ebx, [esp + 0xC]
mov ecx, [esp + 0x10]
mov edx, [esp + 0x14]
mov ebp, [esp + 0x18]
mov esi, [esp + 0x1C]

mov cr3, eax
mov eax, cr4
or eax, 0x10
mov cr4, eax
mov eax, cr0
or eax, 0x80000000
mov cr0, eax

mov esp, edx
push ebx
push cx
lgdt [esp]

pop cx
pop ebx

push ebp
call esi
