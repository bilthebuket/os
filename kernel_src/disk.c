#include "kernel_include/disk.h"
#include "shared_include/disk.h"
#include "shared_include/mem.h"
#include "kernel_include/global.h"

u32 root_lba;

void init_root_lba(u32 lba)
{
	root_lba = lba;
}

u32 get_sector_table_lba(void)
{
	return root_lba;
}

u32 get_num_drive_sectors(void)
{
	return *(u32*) (KERNEL_INFO_VIRTUAL_ADDRESS + DISK_SECTORS_ADDRESS);
}

// for now we only look for one chunk of sectors that can hold all the bytes, rather than breaking the bytes into chunks of sectors
u32 find_available_sectors(u32 num_bytes)
{
	u32 sector_table_lba = get_sector_table_lba();
	u8 buf[SECTOR_SIZE];
	u32 sectors = get_num_drive_sectors();
	for (u32 i = 0; i < sectors / (SECTOR_SIZE * 8); i++)
	{
		if (disk_read_poll(drive_mask, sector_table_lba + i, 1, (u32) buf))
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
			u8 found = 1;
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
				if (sectors_needed / 8)
				{
					if (!(buf[k + sectors_needed / 8] >> (8 - sectors_needed % 8)))
					{
						return (i * SECTOR_SIZE + k) * 8;
					}
					k += sectors_needed / 8 + 1;
				}
				else
				{
					u8 mask = 0xFF << (8 - sectors_needed % 8);
					for (u32 j = 0; j < 8 - sectors_needed % 8; j++)
					{
						if (!(buf[k] & (mask >> j)))
						{
							return (i * SECTOR_SIZE + k) * 8 + j;
						}
					}
					k++;
				}
			}
		}
	}
}

u8 free_sectors(u32 lba, u32 num_sectors)
{
	u32 sector_table_lba = get_sector_table_lba();
	u8 buf[SECTOR_SIZE];
	for (u32 i = 0; i < SECTOR_SIZE; i++)
	{
		buf[i] = 0;
	}
	u32 sectors = get_num_drive_sectors();
	u32 err;
	for (u32 i = 0; i < num_sectors / (SECTOR_SIZE * 8); i++)
	{
		if (err =  disk_write_poll(drive_mask, sector_table_lba + i, 1, (u32) buf))
		{
			return err;
		}
	}
	if (num_sectors % (SECTOR_SIZE * 8) && disk_read_poll(drive_mask, sector_table_lba + num_sectors / (SECTOR_SIZE * 8), 1, (u32) buf))
	{
		for (u32 i = 0; i < num_sectors % (SECTOR_SIZE * 8); i++)
		{
			buf[i / 8] &= ~(0x80 >> (i % 8));
		}
		disk_write_poll(drive_mask, sector_table_lba + num_sectors / (SECTOR_SIZE * 8), 1, (u32) buf);
	}
}

u8 reserve_sectors(u32 lba, u32 num_sectors)
{
	u32 sector_table_lba = get_sector_table_lba();
	u8 buf[SECTOR_SIZE];
	for (u32 i = 0; i < SECTOR_SIZE; i++)
	{
		buf[i] = 0;
	}
	u32 sectors = get_num_drive_sectors();
	u32 err;
	for (u32 i = 0; i < num_sectors / (SECTOR_SIZE * 8); i++)
	{
		if (err = disk_write_poll(drive_mask, sector_table_lba + i, 1, (u32) buf))
		{
			return err;
		}
	}
	if (num_sectors % (SECTOR_SIZE * 8) && disk_read_poll(drive_mask, sector_table_lba + num_sectors / (SECTOR_SIZE * 8), 1, (u32) buf))
	{
		for (u32 i = 0; i < num_sectors % (SECTOR_SIZE * 8); i++)
		{
			buf[i / 8] |= 0x80 >> (i % 8);
		}
		disk_write_poll(drive_mask, sector_table_lba + num_sectors / (SECTOR_SIZE * 8), 1, (u32) buf);
	}

	return 0;
}
