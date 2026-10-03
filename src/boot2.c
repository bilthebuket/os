#include "def.h"
#include "mem.h"
#include "text.h"

__attribute__((used, section(".stage_two_entry")))
void stage_two(smap* smap)
{
	putc(0, 0, 'A', WHITE_TEXT);
	asm("cli");
	asm("hlt");
}
