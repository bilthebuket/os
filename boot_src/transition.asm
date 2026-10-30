bits 32

lgdt [esp + 4]
mov eax, [esp + 8]
mov cr3, eax
mov eax, cr4
or eax, 0x10
mov cr4, eax
mov eax, cr0
or eax, 0x80000001



mov cr0, eax

mov [0xB8000], 'a'
mov [0xB8001], 0xf


mov eax, [esp + 0xC]
mov ebx, [esp + 0x10]
push eax
push bx
lgdt [esp]

pop eax
pop bx
mov eax, [esp + 0x18]
mov esp, [esp + 0x14]
jmp eax
