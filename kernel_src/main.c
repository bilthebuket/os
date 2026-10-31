#include "text.h"
#include "global.h"
#include "mem.h"

void _start(u32 kernel_info_virtual_address)
{
	kernel_info_location = kernel_info_virtual_address;
	u32 text_buf_addr = *(u32*) (kernel_info_location + VGA_TEXT_LOCATION_ADDRESS);
	puts(text_buf_addr , 0,0,(u8*) "hooray",WHITE_TEXT);
	asm("hlt");
}
