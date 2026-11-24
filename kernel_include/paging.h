#ifndef PAGING_KERNEL_H
#define PAGING_KERNEL_H

#include "shared_include/def.h"
#include "shared_include/paging.h"

void map_virtual_to_physical_with_replacement
(
	PageDirectoryEntry* page_table,
	u32 virtual_address,
	u32 size,
	s16 pid,
	u32* physical_addresses
);

#endif
