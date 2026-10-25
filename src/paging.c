#include "paging.h"

void pde_init(PageDirectoryEntry* pde, u16 address)
{
	pde->metadata = 0;
	pde->reserved = 0;
	pde->address = address << (16 - PAGE_ADDRESS_LEN);
	
	pde->metadata |= PAGE_PS;
	pde->metadata |= PAGE_WRITE_THROUGH;
}

u32 map_virtual_to_physical
(
	PageDirectoryEntry* page_table,
	FrameTableEntry* frame_table,
	u32 virtual_address,
	u32 size
)
{
	// the offset in the physical frame needs to match the offset of the virtual address, 
	// thus that offset becomes padding that must be included in the size
	u32 num_pages = size + virtual_address % FRAME_SIZE;
	if ((size + virtual_address % FRAME_SIZE) % FRAME_SIZE != 0)
	{
		num_pages++;
	}

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

		pde_init(&page_table[virtual_address / FRAME_SIZE + i], frame_table_index * FRAME_SIZE);
	}

	return frame_table_index * FRAME_SIZE;
}
