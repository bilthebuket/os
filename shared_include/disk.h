#ifndef DISK_H
#define DISK_H

#define SECTOR_SIZE 512
#define DISK_READ 1
#define DISK_WRITE 2

extern __attribute__((stdcall)) u32 disk_init(void);
extern __attribute__((stdcall)) u32 disk_read_poll(u32 disk_mask, u32 logical_block_address, u32 num_sectors, u32 buf);

#endif
