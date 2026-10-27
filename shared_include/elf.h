#ifndef ELF_H
#define ELF_H

#include "def.h"
#define ELF_SIGNATURE0 0x7F
#define ELF_SIGNATURE1 0x45
#define ELF_SIGNATURE2 0x4C
#define ELF_SIGNATURE3 0x46
#define ELF_32BIT 1
#define ELF_LITTLE_ENDIAN 1

typedef struct __attribute__((packed))
{
	u8 signature[4];
	u8 is32or64;
	u8 endianness;
	u8 unused;
	u16 target_os_info;
	u8 unused2[7];
	u16 object_file_type;
	u16 instruction_set_architecture;
	u32 unused3;
	u32 entry_point;
	u32 program_header_table;
	u32 section_header_table;
	u32 flags;
	u16 header_size;
	u16 pht_entry_size;
	u16 pht_num_entries;
	u16 sht_entry_size;
	u16 sht_num_entries;
	u16 sht_section_name_entry;
} ELFHeader;

typedef struct
{
	u32 type;
	u32 offset;
	u32 virtual_address;
	u32 physical_address;
	u32 file_size;
	u32 memory_size;
	u32 flags;
	u32 alignment;
} ELFProgramHeader;

_Static_assert(sizeof(ELFHeader) == 52, "elf header struct has been padded");
_Static_assert(sizeof(ELFProgramHeader) == 32, "elf program header struct has been padded");

// makes sure its valid, places total size of file in size
u32 elf_read_header(ELFHeader* file, u32* size);

#endif
