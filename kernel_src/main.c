#include "text.h"
#include "global.h"

void _start(u32 kernel_info_virtual_address)
{
	kernel_info_location = kernel_info_virtual_address;
	puts(kernel_info_location, 0,0,(u8*) "hooray",WHITE_TEXT);
	asm("hlt");
}
