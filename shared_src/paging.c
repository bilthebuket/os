#include "paging.h"
#include "mem.h"

void pde_init(PageDirectoryEntry* pde, u16 address)
{
	pde->metadata = 0;
	pde->address = address << (16 - PAGE_ADDRESS_LEN);
	
	pde->metadata |= PAGE_PS;
	pde->metadata |= PAGE_WRITE_THROUGH;
	pde->metadata |= PAGE_PRESENT;
}

u32 map_virtual_to_physical
(
	PageDirectoryEntry* page_table,
	FrameTableEntry* frame_table,
	u32 virtual_address,
	u32 size,
	s16 pid
)
{
	// the offset in the physical frame needs to match the offset of the virtual address, 
	// thus that offset becomes padding that must be included in the size
	u32 virt_start = virtual_address - (virtual_address % FRAME_SIZE);
	u32 virt_end = virtual_address + size;
	virt_end += FRAME_SIZE - (virt_end % FRAME_SIZE);
	u32 num_pages = (virt_end - virt_start) / FRAME_SIZE;

	u32 frame_table_index = 0;
	for (u32 i = 0; i < num_pages; i++)
	{
		while (frame_table_index < FT_NUM_ENTRIES && frame_table[frame_table_index].pid != FRAME_AVAILABLE)
		{
			frame_table_index++;
		}

		if (frame_table_index >= FT_NUM_ENTRIES)
		{
			// todo: handle frame replacement
		}

		pde_init(&page_table[virtual_address / FRAME_SIZE + i], frame_table_index);
		frame_table[frame_table_index].pid = pid;
		frame_table[frame_table_index].pde_index = virtual_address / FRAME_SIZE + i;
	}

	return frame_table_index * FRAME_SIZE;
}
