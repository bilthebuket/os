#ifndef PAGING_H
#define PAGING_H

#define PAGE_PRESENT 1
#define PAGE_READ_WRITE (1 << 1)
#define PAGE_USER_SUPERVISOR (1 << 2)
#define PAGE_WRITE_THROUGH (1 << 3)
#define PAGE_CACHE_DISABLE (1 << 4)
#define PAGE_ACCESSED (1 << 5)
#define PAGE_DIRTY (1 << 6)
#define PAGE_PS (1 << 7)
#define PAGE_GLOBAL (1 << 8)
#define PAGE_PAT (1 << 12)

#define PAGE_ADDRESS_LEN 10

#define PAGING_TABLE_NUM_ENTRIES 1024

#define FRAME_SIZE 0x400000

typedef struct
{
	u16 metadata;
	u16 reserved;
	u32 address;
} PageDirectoryEntry;

typedef struct
{
	u16 pid;
	u16 pde_index;
} FrameTableEntry;

_Static_assert(sizeof(PageDirectoryEntry) == 8, "pde struct has been padded");
_Static_assert(sizeof(FrameTableEntry) == 4, "fte struct has been padded");

void pde_init(PageDirectoryEntry* pde, u16 address);

#endif
