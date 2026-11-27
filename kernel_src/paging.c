#include "kernel_include/paging.h"
#include "kernel_incluide/global.h"
#include "kernel_include/file.h"

// directory with all the page tables not in memory
File page_tables;

u32 init_process_page_table_directory(void)
{
	file_create(&root_directory, "paging", ROOT_DIRECTORY);
	file_open(&page_tables, &root_directory, "paging");
}

u32 free_frame(FrameTableEntry* frame)
{
	if (frame->pid == active_process || frame->pid == ROOT_PROCESS)
	{
		PageDirectoryEntry* page_table;
		if (frame->pid == active_process)
		{
			page_table = (PageDirectoryEntry*) (KERNEL_INFO_VIRTUAL_ADDRESS + ACTIVE_PT_ADDRESS);
		}
		else
		{
			page_table = (PageDirectoryEntry*) (KERNEL_INFO_VIRTUAL_ADDRESS + ROOT_PT_ADDRESS);
		}
		page_table[frame->pde_index].metadata &= ~PRESENT_BIT;
		return 0;
	}
	else
	{
		File f;
		u8 str[NUMBER_MAX_DIGITS];
		int_to_str(frame->pid, str);
		file_open(&f, &page_tables, str);

		PageDirectoryEntry buf[PAGING_TABLE_NUM_ENTRIES];
		u32 err = file_read(drive_mask, buf, sizeof(buf));
		if (err)
		{
			return err;
		}
		buf[frame->pde_index].metadata &= ~PRESENT_BIT;
		return file_write(drive_mask, buf, sizeof(buf));
	}
}

void map_virtual_to_physical_with_replacement
(
	PageDirectoryEntry* page_table,
	u32 virtual_address,
	u32 size,
	s16 pid,
	u32* physical_addresses
)
{
	FrameTableEntry* frame_table = (FrameTableEntry*) (KERNEL_INFO_VIRTUAL_ADDRESS + FT_ADDRESS);
	u32 pages_needed = size / FRAME_SIZE + 1; // not necessarily exactly correct but this is the upper bound for any given size
	u32 pages_free = 0;
	for (u32 i = 0; i < PAGING_TABLE_NUM_ENTRIES; i++)
	{
		if (frame_table[i].pid == FRAME_AVAILABLE)
		{
			pages_free++;
		}
	}
	
	for (u32 i = 0; i < PAGING_TABLE_NUM_ENTRIES && pages_needed > pages_free; i++)
	{
		if (frame_table[i].pid != pid && frame_table[i].pid != FRAME_RESERVED)
		{
			if (free_frame(&frame_table[i]))
			{
				return;
			}
			frame_table[i].pid = FRAME_AVAILABLE;
			pages_free++;
		}
	}
	for (u32 i = 0; i < PAGING_TABLE_NUM_ENTRIES && pages_needed > pages_free; i++)
	{
		if (frame_table[i].pid != FRAME_RESERVED)
		{
			if (free_frame(&frame_table[i]))
			{
				return;
			}
			frame_table[i].pid = FRAME_AVAILABLE;
			pages_free++;
		}
	}

	map_virtual_to_physical(page_table, frame_table, virtual_address, size, pid, physical_addresses);
}
