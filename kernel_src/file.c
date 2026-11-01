#include "file.h"
#include "def.h"
#include "disk.h"

File root_directory;

u8 file_open(File* to_open, File* working_directory, u8* name)
{
	return 0;
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

u8 make_directory(File* parent_directory)
{
	return 0;
}
