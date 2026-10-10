#include "def.h"
#include "mem.h"
#include "text.h"

#define SECTOR_SIZE 512
#define IDT_SIZE 2048

// __attribute__((used...)) makes this the first thing in the binary
// __attribute((stddcall)) makes it so args come off the stack
// this function should never return, the return address is garbage
__attribute__((used, section(".stage_two_entry")))
void __attribute__((stdcall)) stage_two(u32 smap_addr, u16 smap_size, u32 stage_two_location, u16 stage_two_sectors)
{
	smap* smap = (void*) smap_addr;

	u32 stage_two_size = ((u32) stage_two_sectors) * SECTOR_SIZE;
	u32 idt_address = 0;
	for (u16 i = 0; i < smap_size; i++)
	{
		if (smap[i].type == MEM_AVAILABLE)
		{
			u32 address = smap[i].addr_low;
			u32 size = smap[i].length_low;
			if (address < stage_two_location + stage_two_size)
			{
				if (size > stage_two_location + stage_two_size - address)
				{
					size -= stage_two_location + stage_two_size - address;
					address += stage_two_location + stage_two_size - address;
				}
				else
				{
					continue;
				}
			}

			if (size >= IDT_SIZE)
			{
				idt_address = address;
				break;
			}
		}
	}

	putnum(0, 0, idt_address, WHITE_TEXT);
	asm("hlt");
}
