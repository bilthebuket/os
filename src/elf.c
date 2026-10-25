#include "elf.h"

u32 elf_read_header(ELFHeader* file, u32* size);
{
	if
	(
		file->signature != ELF_SIGNATURE ||
		file->is32or64 != ELF_32BIT ||
		file->endianness != ELF_LITTLE_ENDIAN
	)
	{
		return 0;
	}

	u32 r = file->section_header_table;
	r -= sizeof(ELFHeader) + sizeof(ELFProgramHeader) * file->pht_num_entries;
	return r;
}
