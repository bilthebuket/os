#include "paging.h"

void pde_init(PageDirectoryEntry* pde, u16 address)
{
	pde->metadata = 0;
	pde->reserved = 0;
	pde->address = address << (16 - PAGE_ADDRESS_LEN);
	
	pde->metadata |= PAGE_PS;
	pde->metadata |= PAGE_WRITE_THROUGH;
}
