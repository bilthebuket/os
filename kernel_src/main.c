#include "text.h"
#include "global.h"
#include "mem.h"
#include "interrupts.h"
#include "interrupt_driven_disk.h"
#include "disk.h"

void _start(u32 root_lba, u32 __drive_mask)
{
	drive_mask = __drive_mask;
	init_root_directory(root_lba);
	clear_screen();
	puts(0,0,"reached kernel main",WHITE_TEXT);
	setup_interrupts(KERNEL_INFO_VIRTUAL_ADDRESS + IDT_ADDRESS);
spin_lock:
	asm("hlt");
	goto spin_lock;
}
