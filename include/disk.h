#ifndef DISK_H
#define DISK_H

#define SECTOR_SIZE 512
#define DISK_READ 1
#define DISK_WRITE 2

extern __attribute__((stdcall)) u32 request_disk_write(u32 logical_block_address, u8 num_sectors);
extern __attribute__((stdcall)) u32 request_disk_read(u32 logical_block_address, u8 num_sectors);
extern __attribute__((stdcall)) void prep_disk_isr(u32 buffer_address, u32 num_words, u32 operation);
extern void disk_init(void);

#endif
