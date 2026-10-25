#include "def.h"
#include "mem.h"
#include "text.h"
#include "interrupts.h"
#include "disk.h"
#include "paging.h"
#include "process.h"
#include "elf.h"

// __attribute((stddcall)) makes it so args come off the stack
// this function should never return, the return address is garbage
__attribute__((used, section(".stage_two_entry")))
void __attribute__((stdcall)) stage_two(u32 smap_addr, u16 smap_size, u32 stage_two_location, u16 stage_two_sectors, u32 gdt_location)
{
	smap* smap = (void*) smap_addr;
	u32 end_of_smap = smap_addr + smap_size * sizeof(smap);
	u32 kernel_info_physical_location = 0;
	u8 found = 0;

	for (u16 i = 0; i < smap_size; i++)
	{
		if (smap[i].type != MEM_AVAILABLE)
		{
			continue;
		}
		if (smap[i].addr_high != 0)
		{
			continue;
		}
		if (smap[i].length_high != 0)
		{
			smap[i].length_low = 0xFFFFFFFF - smap[i].addr_low;
		}

		u32 address = smap[i].addr_low;
		u32 length = smap[i].length_low;
		if (length < FRAME_SIZE)
		{
			continue;
		}
		if (address < end_of_smap)
		{
			u32 adjustment = end_of_smap - address;
			if (adjustment > length)
			{
				continue;
			}
			address += adjustment;
			length -= adjustment;
		}

		if (stage_two_sectors * SECTOR_SIZE > FRAME_SIZE)
		{
			u32 stage_two_end = stage_two_location + stage_two_sectors * SECTOR_SIZE - 1;
			if ((stage_two_location >= address && stage_two_location < address + length) || (stage_two_end >= address && stage_two_end < address + length))
			{
				u32 adjustment = address - stage_two_end + 1;
				if (adjustment > length)
				{
					continue;
				}
				address += adjustment;
				length -= adjustment;
			}
		}
		else
		{
			u32 stage_two_end = stage_two_location + stage_two_sectors * SECTOR_SIZE - 1;
			u32 frame_end = address + length - 1;
			if ((address >= stage_two_location && address <= stage_two_end) || (frame_end >= stage_two_location && frame_end <= stage_two_end))
			{
				u32 adjustment = address - stage_two_end + 1;
				if (adjustment > length)
				{
					continue;
				}
				address += adjustment;
				length -= adjustment;
			}
		}

		if (address % FRAME_SIZE != 0)
		{
			u32 adjustment = FRAME_SIZE - (address % FRAME_SIZE);
			address += adjustment;
			length -= adjustment;
			if (length < FRAME_SIZE)
			{
				continue;
			}
		}

		kernel_info_physical_location = address;
		found = 1;
		break;
	}

	if (!found)
	{
		asm("hlt");
	}

	u16 frame_index = 0;
	u8* availability_table = (void*) kernel_info_physical_location + FRAME_AVAILABILITY_TABLE_ADDRESS;
	for (u16 i = 0; i < smap_size; i++)
	{
		if (smap[i].addr_high != 0)
		{
			continue;
		}
		u16 num_frames = 0;
		u32 trimmed_length = smap[i].length_low;
		if (smap[i].addr_low % FRAME_SIZE != 0)
		{
			trimmed_length -= FRAME_SIZE - (smap[i].addr_low % FRAME_SIZE);
			num_frames++;
		}
		if ((smap[i].addr_low + smap[i].length_low) % FRAME_SIZE != 0)
		{
			trimmed_length -= (smap[i].addr_low + smap[i].length_low) % FRAME_SIZE;
			num_frames++;
		}

		if (smap[i].type == MEM_AVAILABLE)
		{
			u16 num_frames = smap[i].length_low / FRAME_SIZE;
			for (u16 j = 0; j < num_frames; j++)
			{
				u16 this_index = frame_index + j;
				u8 bitmap = availability_table[this_index / 8];
				bitmap |= 1 << (this_index % 8);
			}
			frame_index += num_frames;
		}
		else
		{
			num_frames += trimmed_length / FRAME_SIZE;
			for (u16 j = 0; j < num_frames; j++)
			{
				u16 this_index = frame_index + j;
				u8 bitmap = availability_table[this_index / 8];
				bitmap &= ~(1 << (this_index % 8));
			}
			frame_index += num_frames;
		}
	}

	u8* new_gdt = (void*) kernel_info_physical_location + GDT_ADDRESS;
	u8* old_gdt = (void*) gdt_location;
	for (u16 i = 0; i < GDT_SIZE; i++)
	{
		new_gdt[i] = old_gdt[i];
	}

	FrameTableEntry* frame_table = (void*) kernel_info_physical_location + FT_ADDRESS;
	for (u16 i = 0; i < FT_NUM_ENTRIES; i++)
	{
		u8 bitmap = availability_table[i / 8];
		if (bitmap & (1 << (i % 8)))
		{
			frame_table[i].pid = FRAME_AVAILABLE;
		}
		else
		{
			frame_table[i].pid = FRAME_RESERVED;
		}
	}

	frame_table[kernel_info_physical_location / FRAME_SIZE].pid = ROOT_PROCESS;
	frame_table[kernel_info_physical_location / FRAME_SIZE].pde_index = ROOT_PAGE_TABLE_ENTRY; // every page table must map virtual 0x0 - 0x400000 to the kernel info frame

	PageDirectoryEntry* root_pt = (void*) kernel_info_physical_location + ROOT_PT_ADDRESS;
	pde_init(&root_pt[ROOT_PAGE_TABLE_ENTRY], kernel_info_physical_location);

	disk_init();

	u8 buf[SECTOR_SIZE];
	if (!disk_read_poll(stage_two_sectors + 1, 1, &buf[0]))
	{
		putnum(0,0,1,WHITE_TEXT);
		asm("hlt");
	}

	ELFHeader* header = (ELFHeader*) &buf[0];
	u32 size;
	if (!elf_read_header(header, &size))
	{
		putnum(0,0,1,WHITE_TEXT);
		asm("hlt");
	}


	setup_interrupts(kernel_info_physical_location + IDT_ADDRESS, stage_two_location);
}
