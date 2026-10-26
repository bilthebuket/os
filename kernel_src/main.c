#include "text.h"

void _start(void)
{
	puts(0,0,(u8*) "hooray",WHITE_TEXT);
	asm("hlt");
}
