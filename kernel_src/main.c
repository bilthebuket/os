#include "text.h"
#include "global.h"
#include "mem.h"

void _start(u32 root_lba)
{
	puts(0,0,(u8*) "hooray",WHITE_TEXT);
	asm("hlt");
}
