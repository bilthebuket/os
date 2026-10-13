#ifndef DISK_H
#define DISK_H

#define SECTOR_SIZE 512

extern __attribute__((stdcall)) u32 request_disk_write(u32 logical_block_address, u8 num_sectors);
extern __attribute__((stdcall)) u32 request_disk_read(u32 logical_block_address, u8 num_sectors);

#endif
