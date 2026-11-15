#include "kernel_include/file.h"
#include "shared_include/def.h"
#include "shared_include/disk.h"
#include "kernel_include/string.h"

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
	FileNode node;
	node.next_lba = working_directory->start_lba;
	node.next_sectors = working_directory->sectors;
	node.flags = working_directory->flags;

	while (1)
	{
		u32 buf_size = node.next_sectors * SECTOR_SIZE;
		u8 buf[buf_size];
		u32 err;
		if (err = disk_read_poll(drive_mask, node.next_lba, buf_size / SECTOR_SIZE, buf))
		{
			return err;
		}
		File* files = &buf[sizeof(FileNode)];
		for (u32 i = 0; i < (num_bytes - sizeof(FileNode)) / sizeof(File); i++)
		{
			if (!strcmp(files[i].name, name))
			{
				found = 1;
				file_copy(to_open, &files[i]);
				return 0;
			}
		}

		node = *(FileNode*) &buf[0];
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
		u32 buf_size = node.next_sectors * SECTOR_SIZE;
		u8 buf[buf_size];
		u32 err;
		if (err = disk_read_poll(drive_mask, node.next_lba, buf_size / SECTOR_SIZE, buf))
		{
			return err;
		}
		
		for (u32 i = sizeof(FileNode); i < buf_size && data_size > 0; i++, data_size--, data = &data[1])
		{
			buf[i] = data[0];
		}

		node = *(FileNode*) &buf[0];
		if (node.flags & LAST_CHUNK_FLAG)
		{
			if (data_size > 0)
			{

			}
			break;
		}
		else
		{
			if (data_size > 0)
			{

			}
			else
			{

			}
		}
	}
}

u8 file_read(File* f, u8* buf, u32 num_bytes)
{
	return 0;
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

u8 make_directory(File* parent_directory, u8* name)
{
	return 0;
}

u8 init_root_directory(u32 lba)
{
	root_directory.head_sector = 0;
	root_directory.header_sector = lba;
	root_directory.header_offset = 0;
	root_directory.head_offset = 0;
}
