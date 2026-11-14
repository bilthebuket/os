#ifndef DISK_KERNEL_H
#define DISK_KERNEL_H

#include "shared_include/def.h"

extern __attribute__((stdcall)) u32 disk_request_read(u32 disk_mask, u32 logical_block_address, u32 num_sectors, u32 buf);
extern __attribute__((stdcall)) u32 disk_request_write(u32 disk_mask, u32 logical_block_address, u32 num_sectors, u32 buf);

// returns the lba of the first sector in a chain of available sectors big enough to fit num_bytes
u32 find_available_sectors(u32 num_bytes);
void free_sectors(u32 lba, u32 num_sectors);
void reserve_sectors(u32 lba, u32 nun_sectors);

#endif
