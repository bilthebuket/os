#ifndef MEM_H
#define MEM_H

#include <assert.h>

typedef struct
{
	unsigned int addr_low;
	unsigned int addr_high;
	unsigned int length_low;
	unsigned int length_high;
	unsigned int type;
} smap;

_Static_assert(sizeof(smap) == 5 * sizeof(unsigned int), "smap struct has been padded (big no no)");

#endif
