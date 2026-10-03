#include "def.h"
#include "mem.h"

void stage_two(void)
{
	asm("mov ax, 0x10");
	asm("mov ds, ax");
	asm("mov es, ax");
	asm("mov fs, ax");
	asm("mov gs, ax");
	asm("mov ss, ax");
}
