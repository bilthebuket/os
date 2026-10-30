#ifndef MEM_H
#define MEM_H

#include <assert.h>
#include "paging.h"

#define MEM_AVAILABLE 1

// first entry in every page directory table must point to the frame that contains this information
#define FRAME_AVAILABILITY_TABLE_ADDRESS 0
#define FRAME_AVAILABILITY_TABLE_SIZE 128
#define GDT_ADDRESS (FRAME_AVAILABILITY_TABLE_ADDRESS + FRAME_AVAILABILITY_TABLE_SIZE)
#define GDT_SIZE 24
#define IDT_SIZE 2048
#define IDT_ADDRESS (GDT_ADDRESS + GDT_SIZE)
#define FT_ADDRESS (IDT_ADDRESS + IDT_SIZE)
#define FT_NUM_ENTRIES 1024
#define FT_SIZE (sizeof(FrameTableEntry) * FT_NUM_ENTRIES)
#define ROOT_PT_ADDRESS (FT_ADDRESS + FT_SIZE)
#define ROOT_PT_SIZE (sizeof(PageDirectoryEntry) * PAGING_TABLE_NUM_ENTRIES)
#define VGA_TEXT_LOCATION_ADDRESS (ROOT_PT_ADDRESS + ROOT_PT_SIZE)
#define VGA_TEXT_LOCATION_SIZE 4

typedef struct
{
	u32 addr_low;
	u32 addr_high;
	u32 length_low;
	u32 length_high;
	u32 type;
} smap;

_Static_assert(sizeof(smap) == 5 * sizeof(u32), "smap struct has been padded (big no no)");

void __attribute__((stdcall)) move_stack(u32 new_location, u32 kernel_info_physical_location, u32 gdt_location, u32 stage_two_sectors, u32 stack_frame);

#endif
