#include "kernel_include/file.h"
#include "shared_include/def.h"
#include "shared_include/disk.h"
#include "kernel_include/string.h"
#include "kernel_include/global.h"
#include "kernel_include/disk.h"

File root_directory;

void file_copy(File* to, File* from)
{
	to->name = from->name;
	to->start_lba = from->start_lba;
	to->sectors = from->sectors;
	to->flags = from->flags;
}

u8 file_open(File* to_open, File* working_directory, u8* name)
{
	if (!(working_directory->flags & DIRECTORY_FLAG))
	{
		return DIRECTORY_ERROR;
	}

	FileNode node;
	node.next_lba = working_directory->start_lba;
	node.next_sectors = working_directory->sectors;
	node.flags = working_directory->flags;

	while (1)
	{
		u32 buf_size = node.next_sectors * SECTOR_SIZE;
		u8 buf[buf_size];
		u32 err;
		if (err = disk_read_poll(drive_mask, node.next_lba, buf_size / SECTOR_SIZE, (u32) buf))
		{
			return err;
		}
		File* files = (File*) &buf[0];
		node = *(FileNode*) &buf[buf_size - sizeof(FileNode)];
		for (u32 i = 0; i < node.num_bytes / sizeof(File); i++)
		{
			if (!strcmp(files[i].name, name))
			{
				file_copy(to_open, &files[i]);
				return 0;
			}
		}

		if (node.flags & LAST_CHUNK_FLAG)
		{
			return FILE_NOT_FOUND;
		}
	}
}

u8 file_write(File* f, u8* data, u32 data_size)
{
	FileNode node;
	node.next_lba = f->start_lba;
	node.next_sectors = f->sectors;
	node.flags = f->flags;

	while (1)
	{
		u8 buf[SECTOR_SIZE];
		u32 err;
		u32 lba = node.next_lba;
		u32 sectors = node.next_sectors;
		if (err = disk_read_poll(drive_mask, lba + sectors - 1, 1, (u32) buf))
		{
			return err;
		}

		FileNode* next = (FileNode*) &buf[SECTOR_SIZE - sizeof(FileNode)];

		if (data_size < sectors * SECTOR_SIZE - sizeof(FileNode))
		{
			if (err = disk_write_poll(drive_mask, lba, data_size / SECTOR_SIZE, (u32) data))
			{
				return err;
			}
			for (u32 i = 0; i < data_size % SECTOR_SIZE; i++)
			{
				buf[i] = data[(data_size / SECTOR_SIZE) * SECTOR_SIZE + i];
			}
			next->num_bytes = data_size;
			next->flags |= LAST_CHUNK_FLAG;
			if (err = disk_write_poll(drive_mask, lba + data_size / SECTOR_SIZE, 1, (u32) buf))
			{
				return err;
			}

			if (sectors > data_size / SECTOR_SIZE + 1)
			{
				free_sectors(lba + data_size / SECTOR_SIZE + 1, sectors - (data_size / SECTOR_SIZE + 1));
			}
			node = *next;
			do
			{
				lba = node.next_lba;
				sectors = node.next_sectors;
				if (err = disk_read_poll(drive_mask, lba + sectors - 1, 1, (u32) buf))
				{
					return err;
				}
				free_sectors(lba, sectors);
				node = *(FileNode*) &buf[SECTOR_SIZE - sizeof(FileNode)];
			}
			while (!(node.flags & LAST_CHUNK_FLAG));
			return 0;
		}
		else if (data_size > sectors * SECTOR_SIZE - sizeof(FileNode))
		{
			u32 amount_writing = sectors * SECTOR_SIZE - sizeof(FileNode);
			next->num_bytes = amount_writing;
			for (u32 i = 0; i < SECTOR_SIZE - sizeof(FileNode); i++)
			{
				buf[i] = data[(sectors - 1) * SECTOR_SIZE + i];
			}
			if (next->flags & LAST_CHUNK_FLAG)
			{
				u32 next_lba = find_available_sectors(data_size - amount_writing + sizeof(FileNode));
				if (!next_lba)
				{
					return DISK_FULL;
				}
				next->flags &= ~LAST_CHUNK_FLAG;
				next->next_lba = next_lba;
				next->next_sectors = (data_size - amount_writing + sizeof(FileNode)) / SECTOR_SIZE;
				reserve_sectors(next->next_lba, next->next_sectors);
				if ((data_size - amount_writing + sizeof(FileNode)) % SECTOR_SIZE)
				{
					next->next_sectors++;
				}

				if ((err = disk_write_poll(drive_mask, lba, sectors - 1, (u32) data)) ||
				    (err = disk_write_poll(drive_mask, lba + sectors - 1, 1, (u32) buf)))
				{
					return err;
				}

				node = *next;
				data_size -= amount_writing;
				data = &data[amount_writing];

				for (u32 i = 0; i < data_size % SECTOR_SIZE; i++)
				{
					buf[i] = data[data_size - data_size % SECTOR_SIZE + i];
				}
				next = (FileNode*) &buf[SECTOR_SIZE - sizeof(FileNode)];
				next->num_bytes = data_size;
				next->flags = LAST_CHUNK_FLAG;

				if ((err = disk_write_poll(drive_mask, next_lba, data_size / SECTOR_SIZE, (u32) data)) ||
				    (err = disk_write_poll(drive_mask, next_lba + data_size / SECTOR_SIZE, 1, (u32) buf)))
				{
					return err;
				}
				return 0;
			}
			else
			{
				if ((err = disk_write_poll(drive_mask, lba, sectors - 1, (u32) data)) ||
				    (err = disk_write_poll(drive_mask, lba + sectors - 1, 1, (u32) buf)))
				{
					return err;
				}

				node = *next;
				data_size -= amount_writing;
				data = &data[amount_writing];
			}
		}
		else
		{
			u32 amount_writing = sectors * SECTOR_SIZE - sizeof(FileNode);
			next->num_bytes = amount_writing;
			for (u32 i = 0; i < SECTOR_SIZE - sizeof(FileNode); i++)
			{
				buf[i] = data[(sectors - 1) * SECTOR_SIZE + i];
			}
			if ((err = disk_write_poll(drive_mask, next->next_lba, data_size / SECTOR_SIZE, (u32) data)) ||
			    (err = disk_write_poll(drive_mask, next->next_lba + data_size / SECTOR_SIZE, 1, (u32) buf)))
			{
				return err;
			}
			return 0;
		}
	}
}

u32 file_read(File* f, u8* buf, u32 num_bytes)
{
	FileNode node;
	node.next_lba = f->start_lba;
	node.next_sectors = f->sectors;
	node.flags = f->flags;
	u32 r = 0;

	while (1)
	{
		u32 err;
		u8 buf2[SECTOR_SIZE];
		if (err = disk_read_poll(drive_mask, node.next_lba + node.next_sectors - 1, 1, (u32) buf2))
		{
			return r;
		}
		FileNode next = *(FileNode*) &buf2[SECTOR_SIZE - sizeof(FileNode)];
		if (next.num_bytes <= num_bytes)
		{
			if (err = disk_read_poll(drive_mask, node.next_lba, num_bytes / SECTOR_SIZE, (u32) buf))
			{
				return r;
			}
			r += (num_bytes / SECTOR_SIZE) * SECTOR_SIZE;
			if (num_bytes % SECTOR_SIZE)
			{
				if (err = disk_read_poll(drive_mask, node.next_lba + num_bytes / SECTOR_SIZE, 1, (u32) buf2))
				{
					return r;
				}
				for (u32 i = 0; i < num_bytes % SECTOR_SIZE; i++)
				{
					buf[(num_bytes / SECTOR_SIZE) * SECTOR_SIZE + i] = buf2[i];
				}
				r += num_bytes % SECTOR_SIZE;
			}
			return r;
		}
		else
		{
			if (err = disk_read_poll(drive_mask, node.next_lba, next.num_bytes / SECTOR_SIZE + 1, (u32) buf))
			{
				return r;
			}
			r += next.num_bytes;
			num_bytes -= next.num_bytes;
			node = next;
			if (node.flags & LAST_CHUNK_FLAG)
			{
				return r;
			}
		}
	}
}

u8 file_move_head(File* f, u32 location)
{
	return 0;
}

u32 file_get_head(File* f)
{
	return 0;
}

u32 file_get_size(File* f)
{
	return 0;
}

u8 file_create(File* directory, u8* name, u8 flags)
{
	return 0;
}

u8 init_root_directory(u32 lba)
{
	root_directory.start_lba = lba;
	root_directory.sectors = 0; // this should instead be initialzed by reading the filenode at start_lba
	root_directory.flags = DIRECTORY_FLAG;
}
