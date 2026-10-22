#ifndef MEM_H
#define MEM_H

#include <assert.h>
#include "interrupts.h"

#define MEM_AVAILABLE 1

// first entry in every page directory table must point to the frame that contains this information
#define FRAME_AVAILABILITY_TABLE_ADDRESS 0
#define FRAME_AVAILABILITY_TABLE_SIZE 128
#define GDT_ADDRESS (FRAME_AVAILABILITY_TABLE_ADDRESS + FRAME_AVAILABILITY_TABLE_SIZE)
#define GDT_SIZE 24
#define IDT_ADDRESS (GDT_ADDRESS + GDT_SIZE)
#define FT_ADDRESS (IDT_ADDRESS + IDT_SIZE)
#define FT_SIZE (sizeof(FrameTableEntry) * 1024)

typedef struct
{
	u32 addr_low;
	u32 addr_high;
	u32 length_low;
	u32 length_high;
	u32 type;
} smap;

_Static_assert(sizeof(smap) == 5 * sizeof(u32), "smap struct has been padded (big no no)");

#endif
