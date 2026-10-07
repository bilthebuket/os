#ifndef MEM_H
#define MEM_H

#include <assert.h>

#define MEM_AVAILABLE 1

typedef struct
{
	u32 addr_low;
	u32 addr_high;
	u32 length_low;
	u32 length_high;
	u32 type;
} smap;

_Static_assert(sizeof(smap) == 5 * sizeof(unsigned int), "smap struct has been padded (big no no)");

#endif
