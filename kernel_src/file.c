#include "file.h"
#include "def.h"
#include "disk.h"
#include "stirng.h"

File root_directory;

u8 file_open(File* to_open, File* working_directory, u8* name)
{
	u8 buf[SECTOR_SIZE];
	u32 err;
	if (err = disk_read_poll(drive_mask, working_directory->header_sector, 1, &buf[0]))
	{
		return (u8) err;
	}

	FileHead dir_head = *(FileHead*) &buf[working_directory->header_offset];
	if (dir_head.start_sector !=
}

u8 file_write(File* f, u8* data, u32 data_size)
{
	return 0;
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
