#include "interrupts.h"
#include "text.h"

extern void default_isr(void);
extern void __attribute__((stdcall)) call_lidt(u32);

void load_idt(u32 address, u32 code_location)
{
	InterruptDescriptor* table = (void*) address;
	u32 num_entries = IDT_SIZE / sizeof(InterruptDescriptor);

	InterruptDescriptor id;
	id.offset_low = (((u32) &default_isr) + code_location) & 0xFFFF;
	id.offset_high = (((u32) &default_isr) + code_location) >> 16;
	id.segment_selector = 0x8; // code segment, use gdt instead of ldt, ring 0 privilege level
	id.attributes = (1 << 7) + 0xE; // sets present bit, only ring 0 can access interrupt, sets gate type to 32 bit interrupt gate

	for (u32 i = 0; i < num_entries; i++)
	{
		table[i] = id;
	}

	call_lidt(address);
	asm("sti");
}
