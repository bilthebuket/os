#ifndef ELF_H
#define ELF_H

#include "def.h"

#define ELF_SIGNATURE 0x7F454C46
#define ELF_32BIT 1
#define ELF_LITTLE_ENDIAN 1

typedef struct
{
	u32 signature;
	u8 is32or64;
	u8 endianness;
	u8 unused;
	u16 target_os_info;
	u8 unused2;
	u16 object_file_type;
	u16 instruction_set_architecture;
	u32 unused3;
	u32 entry_point;
	u32 program_header_table;
	u32 section_header_table;
	u32 flags;

#endif
