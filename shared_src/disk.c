#include "shared_include/def.h"
#include "shared_include/disk.h"

u32 get_padded_buf_size(u16 offset, u32 num_bytes)
{
	return num_bytes + (offset % SECTOR_SIZE) + SECTOR_SIZE - (offset + num_bytes) % SECTOR_SIZE;
}
