#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "def.h"

#define IDT_SIZE 2048

typedef struct
{
	u16 offset_low;
	u16 segment_selector;
	u8 reserved;
	u8 attributes;
	u16 offset_high;
} InterruptDescriptor;

_Static_assert(sizeof(InterruptDescriptor) == 8, "interrupt descriptor struct has been padded (big no no)");

void load_idt(u32 address, u32 code_location);

#endif
