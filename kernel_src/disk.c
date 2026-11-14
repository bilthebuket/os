#include "kernel_include/disk.h"
#include "shared_include/mem.h"

u32 get_sector_table_lba(void)
{
	return root_lba;
}

u32 get_num_drive_sectors(void)
{
	return *(u32*) DISK_SECTORS_ADDRESS;
}

u32 find_available_sectors(u32 num_bytes)
{
	u32 sector_table_lba = get_sector_table_lba();
	u8 buf[SECTOR_SIZE];
	u32 sectors = get_num_drive_sectors();
	for (u32 i = 0; i < sectors / (SECTOR_SIZE * 8); i++)
	{
		if (disk_read_poll(drive_mask, sector_table_lba + i, 1, buf))
		{
			return 0;
		}
		u32 sectors_needed = num_bytes / SECTOR_SIZE;
		if (num_bytes % SECTOR_SIZE)
		{
			sectors_needed++;
		}

		for (u32 k = 0; k < SECTOR_SIZE;)
		{
			found = 1;
			for (u32 j = 0; j < sectors_needed / 8; j++)
			{
				if (!buf[k + j])
				{
					found = 0;
					k = j + 1;
					break;
				}
			}

			if (found)
			{
				if (!(buf[k + sectors_needed / 8] >> (sectors_needed % 8)))
				{
					return (i * SECTOR_SIZE + k) * 8;
				}
			}
		}
	}
}

void free_sectors(u32 lba, u32 num_sectors)
{
}

void reserve_sectors(u32 lba, u32 nun_sectors)
{

}
