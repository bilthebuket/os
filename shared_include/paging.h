#ifndef PAGING_H
#define PAGING_H

#include "def.h"

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

#define FRAME_AVAILABLE -1
#define FRAME_RESERVED -2

#define PDE_UNINITIALIZED 0xFFFF
#define PDE_INITIALIZED 0

typedef union
{
	u32 combined;
	struct
	{
		u16 metadata;
		u16 address;
	};
} PageDirectoryEntry;

typedef struct
{
	s16 pid;
	u16 pde_index;
} FrameTableEntry;

_Static_assert(sizeof(PageDirectoryEntry) == 4, "pde struct has been padded");
_Static_assert(sizeof(FrameTableEntry) == 4, "fte struct has been padded");

// address is the (physical address of the start of the frame) >> 22, so really its just the index in the frame table
void pde_init(PageDirectoryEntry* pde, u16 address);
// returns physical address
void map_virtual_to_physical
(
	PageDirectoryEntry* page_table,
	FrameTableEntry* frame_table,
	u32 virtual_address,
	u32 size,
	s16 pid,
	u32* physical_addresses
);

u32 __attribute__((stdcall)) paging_enabled_huh(void);

#endif
