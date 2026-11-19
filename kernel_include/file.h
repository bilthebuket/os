#ifndef FILE_H
#define FILE_H

#include "shared_include/def.h"

#define DIRECTORY_FLAG 1
#define LAST_CHUNK_FLAG (1 << 1)

#define FILE_NOT_FOUND 1
#define DIRECTORY_ERROR 2
#define DISK_FULL 3

typedef struct
{
	u8* name;
	u32 start_lba;
	u32 sectors;
	u8 flags;
} File;

typedef struct
{
	u32 num_bytes;
	u32 next_lba;
	u32 next_sectors;
	u8 flags;
} FileNode;

extern File root_directory;

u8 file_open(File* to_open, File* working_directory, u8* name);
u8 file_write(File* f, u8* data, u32 data_size);
u32 file_read(File* f, u8* buf, u32 num_bytes);
u8 file_move_head(File* f, u32 location);
u32 file_get_head(File* f);
u32 file_get_size(File* f);
u8 file_create(File* directory, u8* name, u8 flags);
u8 init_root_directory(u32 lba);

#endif
