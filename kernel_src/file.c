#include "kernel_include/file.h"
#include "shared_include/def.h"
#include "shared_include/disk.h"
#include "kernel_include/string.h"
#include "kernel_include/global.h"
#include "kernel_include/disk.h"

File root_directory;

void file_copy(File* to, File* from)
{
	u32 i = 0;
	for (; from->name[i] != '\0'; i++)
	{
		to->name[i] = from->name[i];
	}
	to->name[i] = '\0';
	to->start_lba = from->start_lba;
	to->sectors = from->sectors;
	to->flags = from->flags;
}

u32 file_open_helper(File* to_open, FileNode* node, u8* name)
{
	u32 buf_size = node->next_sectors * SECTOR_SIZE;
	u8 buf[buf_size];
	u32 err;
	if (err = disk_read_poll(drive_mask, node->next_lba, node->next_sectors, (u32) buf))
	{
		return err;
	}
	File* files = (File*) &buf[0];
	FileNode* next = (FileNode*) &buf[buf_size - sizeof(FileNode)];
	for (u32 i = 0; i < next->num_bytes / sizeof(File); i++)
	{
		if (!strcmp(files[i].name, name))
		{
			file_copy(to_open, &files[i]);
			return 0;
		}
	}

	if (next->flags & LAST_CHUNK_FLAG)
	{
		return FILE_NOT_FOUND;
	}
	return file_open_helper(to_open, next, name);
}

u32 file_open(File* to_open, File* working_directory, u8* name)
{
	if (!(working_directory->flags & DIRECTORY_FLAG))
	{
		return DIRECTORY_ERROR;
	}

	FileNode node;
	node.next_lba = working_directory->start_lba;
	node.next_sectors = working_directory->sectors;
	node.flags = working_directory->flags;
	return file_open_helper(to_open, &node, name);
}

void recursive_free_sectors(FileNode* node)
{
	if (node->flags & LAST_CHUNK_FLAG)
	{
		return;
	}

	u8 buf[SECTOR_SIZE];
	if (disk_read_poll(drive_mask, node->next_lba + node->next_sectors - 1, 1, (u32) buf))
	{
		return;
	}
	free_sectors(node->next_lba, node->next_sectors);
	recursive_free_sectors((FileNode*) &buf[SECTOR_SIZE - sizeof(FileNode)]);
}

u32 allocate_file_sectors(FileNode* node, u32 num_bytes)
{
	if (num_bytes % SECTOR_SIZE)
	{
		num_bytes += SECTOR_SIZE - num_bytes % SECTOR_SIZE;
	}

	u32 lba = find_available_sectors(num_bytes);
	if (!lba)
	{
		return DISK_FULL;
	}

	node->next_lba = lba;
	node->next_sectors = num_bytes / SECTOR_SIZE;

	u8 buf[SECTOR_SIZE];
	FileNode* next = (FileNode*) &buf[SECTOR_SIZE - sizeof(FileNode)];
	next->num_bytes = 0;
	next->flags = LAST_CHUNK_FLAG;
	return disk_write_poll(drive_mask, lba + num_bytes / SECTOR_SIZE - 1, 1, (u32) buf);
}

u32 file_write_helper(FileNode* node, u8* data, u32 data_size);

u32 handle_write_excess(FileNode* node, u8* data, u32 data_size, u8* last_sector_buf)
{
	FileNode* next = (FileNode*) &last_sector_buf[SECTOR_SIZE - sizeof(FileNode)];
	u32 err;
	if (next->num_bytes / SECTOR_SIZE < (node->next_sectors - 1) * SECTOR_SIZE)
	{
		if (data_size > 0)
		{
			u8 buf[SECTOR_SIZE];
			for (u32 i = 0; i < data_size; i++)
			{
				buf[i] = data[i];
			}
			if (err = disk_write_poll(drive_mask, node->next_lba + next->num_bytes / SECTOR_SIZE, 1, (u32) buf))
			{
				return err;
			}
			next->num_bytes += data_size;
			data_size = 0;
		}
	}
	else
	{
		u32 i = 0;
		for (; i < data_size % SECTOR_SIZE && i < SECTOR_SIZE - sizeof(FileNode); i++)
		{
			last_sector_buf[i] = data[i];
		}
		data = &data[i];
		data_size -= i;
		next->num_bytes += i;
	}

	if (data_size == 0)
	{
		recursive_free_sectors(next);
		next->flags |= LAST_CHUNK_FLAG;
		return disk_write_poll(drive_mask, node->next_lba + node->next_sectors - 1, 1, (u32) last_sector_buf);
	}

	if (next->flags & LAST_CHUNK_FLAG)
	{
		if (err = allocate_file_sectors(next, data_size))
		{
			return err;
		}
		next->flags &= ~LAST_CHUNK_FLAG;
		if (err = disk_write_poll(drive_mask, node->next_lba + node->next_sectors - 1, 1, (u32) last_sector_buf))
		{
			return err;
		}
	}

	return file_write_helper(next, data, data_size);
}

u32 file_write_helper(FileNode* node, u8* data, u32 data_size)
{
	u8 buf[SECTOR_SIZE];
	u32 err;
	if (err = disk_read_poll(drive_mask, node->next_lba + node->next_sectors - 1, 1, (u32) buf))
	{
		return err;
	}

	FileNode* next = (FileNode*) &buf[SECTOR_SIZE - sizeof(FileNode)];
	if (data_size / SECTOR_SIZE <= node->next_sectors - 1)
	{
		if (err = disk_write_poll(drive_mask, node->next_lba, data_size / SECTOR_SIZE, (u32) data))
		{
			return err;
		}
		next->num_bytes = (data_size / SECTOR_SIZE) * SECTOR_SIZE;
		data = &data[next->num_bytes];
		data_size -= next->num_bytes;
	}
	else
	{
		if (err = disk_write_poll(drive_mask, node->next_lba, node->next_sectors - 1, (u32) data))
		{
			return err;
		}
		next->num_bytes = (node->next_sectors - 1) * SECTOR_SIZE;
		data = &data[next->num_bytes];
		data_size -= next->num_bytes;
	}

	handle_write_excess(node, data, data_size, buf);
}

u32 file_write(File* f, u8* data, u32 data_size)
{
	FileNode node;
	node.next_sectors = f->sectors;
	node.next_lba = f->start_lba;
	node.flags = 0;
	return file_write_helper(&node, data, data_size);
}

u32 file_read_helper(FileNode* node, u8* buf, u32 num_bytes, u32* r)
{
	if (node->flags & LAST_CHUNK_FLAG)
	{
		return 0;
	}

	u8 buf2[SECTOR_SIZE];
	u32 err;
	if (err = disk_read_poll(drive_mask, node->next_lba + node->next_sectors - 1, 1, (u32) buf2))
	{
		return err;
	}

	FileNode* next = (FileNode*) &buf2[SECTOR_SIZE - sizeof(FileNode)];
	if (next->num_bytes <= num_bytes)
	{
		if (err = disk_read_poll(drive_mask, node->next_lba, next->num_bytes / SECTOR_SIZE, (u32) buf))
		{
			return err;
		}
		*r += (next->num_bytes / SECTOR_SIZE) * SECTOR_SIZE;

		if (next->num_bytes / SECTOR_SIZE != node->next_lba + node->next_sectors - 1)
		{
			if (err = disk_read_poll(drive_mask, node->next_lba + next->num_bytes / SECTOR_SIZE, 1, (u32) buf2))
			{
				return err;
			}
		}
		for (u32 i = 0; i < next->num_bytes % SECTOR_SIZE; i++)
		{
			buf[i + (next->num_bytes / SECTOR_SIZE) * SECTOR_SIZE] = buf2[i];
		}
		*r += next->num_bytes % SECTOR_SIZE;
		num_bytes -= next->num_bytes;
		buf = &buf[next->num_bytes];
		return file_read_helper(next, buf, num_bytes, r);
	}
	else
	{
		if (err = disk_read_poll(drive_mask, node->next_lba, num_bytes / SECTOR_SIZE, (u32) buf))
		{
			return err;
		}
		*r += num_bytes / SECTOR_SIZE;
		
		if (num_bytes / SECTOR_SIZE != node->next_lba + node->next_sectors - 1)
		{
			if (err = disk_read_poll(drive_mask, node->next_lba + num_bytes / SECTOR_SIZE, 1, (u32) buf2))
			{
				return err;
			}
		}
		for (u32 i = 0; i < num_bytes % SECTOR_SIZE; i++)
		{
			buf[i + (num_bytes / SECTOR_SIZE) * SECTOR_SIZE] = buf2[i];
		}
		*r += num_bytes % SECTOR_SIZE;
		return 0;
	}
}

u32 file_read(File* f, u8* buf, u32 num_bytes)
{
	FileNode node;
	node.next_lba = f->start_lba;
	node.next_sectors = f->sectors;
	node.flags = f->flags;
	u32 r = 0;
	file_read_helper(&node, buf, num_bytes, &r);
	return r;
}

u32 file_move_head(File* f, u32 location)
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

u32 file_create_helper(FileNode* node, u8* name, u8 flags)
{
	u8 buf[SECTOR_SIZE];
	u32 err;
	if (err = disk_read_poll(drive_mask, node->next_lba + node->next_sectors - 1, 1, (u32) buf))
	{
		return err;
	}
	FileNode* next = (FileNode*) &buf[SECTOR_SIZE - sizeof(FileNode)];
	u32 available_space = node->next_sectors * SECTOR_SIZE - sizeof(FileNode) - next->num_bytes;

	File f;
	FileNode n;
	if (err = allocate_file_sectors(&n, SECTOR_SIZE))
	{
		return err;
	}
	f.start_lba = n.next_lba;
	f.sectors = 1;
	f.flags = flags;
	u32 i = 0;
	for (; i < NAME_MAX_LENGTH && name[i] != '\0'; i++)
	{
		f.name[i] = name[i];
	}
	f.name[i] = '\0';

	if (available_space >= sizeof(File))
	{
		u8* f_bytes = (u8*) &f;
		if (next->num_bytes >= (node->next_sectors - 1) * SECTOR_SIZE)
		{
			for (u32 i = 0; i < sizeof(File); i++)
			{
				buf[next->num_bytes % SECTOR_SIZE + i] = f_bytes[i];
			}
		}
		else if (next->num_bytes + sizeof(File) > (node->next_sectors - 1) * SECTOR_SIZE)
		{
			u8 buf2[SECTOR_SIZE];
			if (err = disk_read_poll(drive_mask, node->next_lba + next->num_bytes / SECTOR_SIZE, 1, (u32) buf2))
			{
				return err;
			}
			u32 i = 0;
			for (; i < SECTOR_SIZE - next->num_bytes % SECTOR_SIZE; i++)
			{
				buf2[next->num_bytes % SECTOR_SIZE + i] = f_bytes[i];
			}
			for (; i < sizeof(File); i++)
			{
				buf[i - SECTOR_SIZE + next->num_bytes % SECTOR_SIZE] = f_bytes[i];
			}
			if (err = disk_write_poll(drive_mask, node->next_lba + next->num_bytes / SECTOR_SIZE, 1, (u32) buf2))
			{
				return err;
			}
		}
		else
		{
			u8 buf2[SECTOR_SIZE * 2];
			if (err = disk_read_poll(drive_mask, node->next_lba + next->num_bytes / SECTOR_SIZE, 2, (u32) buf2))
			{
				return err;
			}
			for (u32 i = 0; i < sizeof(File); i++)
			{
				buf2[next->num_bytes % SECTOR_SIZE + i] = f_bytes[i];
			}
			if (err = disk_write_poll(drive_mask, node->next_lba + next->num_bytes / SECTOR_SIZE, 2, (u32) buf2))
			{
				return err;
			}
		}
		next->num_bytes += sizeof(File);
		return disk_write_poll(drive_mask, node->next_lba + node->next_sectors - 1, 1, (u32) buf);
	}

	if (next->flags & LAST_CHUNK_FLAG)
	{
		if (err = allocate_file_sectors(next, sizeof(File)))
		{
			return err;
		}
		return file_write_helper(next, (u8*) &f, sizeof(File));
	}

	return file_create_helper(next, name, flags);

}

u32 file_create(File* directory, u8* name, u8 flags)
{
	FileNode node;
	node.next_sectors = directory->sectors;
	node.next_lba = directory->start_lba;
	node.flags = 0;
	return file_create_helper(&node, name, flags);
}

u32 init_root_directory(u32 lba)
{
	root_directory.start_lba = lba;
	root_directory.sectors = 1;
	root_directory.flags = DIRECTORY_FLAG;

	u8 buf[SECTOR_SIZE];
	FileNode* node = (FileNode*) &buf[SECTOR_SIZE - sizeof(FileNode)];
	node->num_bytes = 0;
	node->flags = LAST_CHUNK_FLAG;
	u32 err;
	if (err = disk_write_poll(drive_mask, lba, 1, (u32) buf))
	{
		return err;
	}
	return reserve_sectors(lba, 1);
}
