#include "elf.h"
#include "text.h"

u32 elf_read_header(ELFHeader* file, u32* size)
{
	if
	(
		file->signature[0] != ELF_SIGNATURE0 ||
		file->signature[1] != ELF_SIGNATURE1 ||
		file->signature[2] != ELF_SIGNATURE2 ||
		file->signature[3] != ELF_SIGNATURE3 ||
		file->is32or64 != ELF_32BIT ||
		file->endianness != ELF_LITTLE_ENDIAN
	)
	{
		putnum(0,0,file->signature[0],WHITE_TEXT);
		putnum(1,0,file->signature[1],WHITE_TEXT);
		putnum(2,0,file->signature[2],WHITE_TEXT);
		putnum(3,0,file->signature[3],WHITE_TEXT);
		asm("hlt");
		return 0;
	}

	u32 r = file->section_header_table;
	r += file->sht_num_entries * file->sht_entry_size;
	*size = r;
	return 1;
}
