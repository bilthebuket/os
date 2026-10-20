#include "def.h"
#include "mem.h"
#include "text.h"
#include "interrupts.h"
#include "disk.h"

// __attribute((stddcall)) makes it so args come off the stack
// this function should never return, the return address is garbage
__attribute__((used, section(".stage_two_entry")))
void __attribute__((stdcall)) stage_two(u32 smap_addr, u16 smap_size, u32 stage_two_location, u16 stage_two_sectors)
{
	smap* smap = (void*) smap_addr;

	u32 end_of_smap = smap_addr + smap_size * sizeof(smap);
	u32 idt_address = 0;
	for (u16 i = 0; i < smap_size; i++)
	{
		if (smap[i].type == MEM_AVAILABLE)
		{
			u32 address = smap[i].addr_low;
			u32 size = smap[i].length_low;

			// moving idt location past everything else we have in memory so far
			// (boot2 binary, the stack, boot1 binary, smap)
			if (address < end_of_smap)
			{
				u32 diff = end_of_smap - address;
				if (size < diff)
				{
					continue;
				}

				address += diff;
				size -= diff;
			}

			if (size >= IDT_SIZE)
			{
				idt_address = address;
				break;
			}
		}
	}
	
	disk_init();
	setup_interrupts(idt_address, stage_two_location);
	u8 stuff[SECTOR_SIZE * 2];
	for (u32 i = 0; i < SECTOR_SIZE * 2; i++)
	{
		stuff[i] = 'A';
	}
	if (!request_disk_write(0, 2, (u32) &stuff[0]))
	{
		putnum(0,0,1,WHITE_TEXT);
		asm("cli");
		asm("hlt");
	}
idle:
	asm("hlt");
	goto idle;
}
