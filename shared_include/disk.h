#ifndef DISK_H
#define DISK_H

#define SECTOR_SIZE 512
#define DISK_READ 1
#define DISK_WRITE 2

extern __attribute__((stdcall)) u32 disk_init(u32 disk_sectors_address);
extern __attribute__((stdcall)) u32 disk_read_poll(u32 disk_mask, u32 logical_block_address, u32 num_sectors, u32 buf);
extern __attribute__((stdcall)) u32 disk_write_poll(u32 disk_mask, u32 logical_block_address, u32 num_sectors, u32 buf);

// returns how big a buffer needs to be to hold the disk sectors that contain a number of bytes starting at an offset within a sector
u32 get_padded_buf_size(u16 offset, u32 num_bytes);

#endif
