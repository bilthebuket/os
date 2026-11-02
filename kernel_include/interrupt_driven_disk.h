#ifndef INTERRUPT_DRIVEN_DISK
#define INTERRUPT_DRIVEN_DISK

extern __attribute__((stdcall)) u32 disk_request_read(u32 disk_mask, u32 logical_block_address, u32 num_sectors, u32 buf);
extern __attribute__((stdcall)) u32 disk_request_write(u32 disk_mask, u32 logical_block_address, u32 num_sectors, u32 buf);

#endif
