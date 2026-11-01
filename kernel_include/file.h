#ifndef FILE_H
#define FILE_H

#include "def.h"

#define FILE 0
#define DIRECTORY 1

typedef struct FileHead
{
	u8* name;
	u32 start_sector;
	u16 start_offset;
	u8 type;
} FileHead;

typedef struct FileNode
{
	u32 num_bytes;
	u32 next_sector;
	u16 next_offset;
} FileNode;

typedef struct File
{
	// head = where we are reading/writing
	// header = FileHead
	u32 head_sector;
	u32 header_sector;
	u16 header_offset;
	u16 head_offset;
} File;

extern File root_directory;

u8 file_open(File* to_open, File* working_directory, u8* name);
u8 file_write(File* f, u8* data, u32 data_size);
u8 file_read(File* f, u8* buf, u32 num_bytes);
u8 file_move_head(File* f, u32 location);
u32 file_get_head(File* f);
u32 file_get_size(File* f);
u8 make_directory(File* parent_directory);

#endif
