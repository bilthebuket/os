#include "text.h"
#include "global.h"
#include "mem.h"
#include "interrupts.h"
#include "interrupt_driven_disk.h"
#include "disk.h"

extern void display_esp(void);

void _start(u32 root_lba, u32 drive_mask)
{
	clear_screen();
	puts(0,0,"reached kernel main",WHITE_TEXT);
	setup_interrupts(KERNEL_INFO_VIRTUAL_ADDRESS + IDT_ADDRESS);
	u8 buf[SECTOR_SIZE * 2];
	if (disk_request_read(drive_mask, 0, 2, (u32) &buf[0]))
	{
		puts(0,0,(u8*)"shit failed",WHITE_TEXT);
	}
spin_lock:
	asm("hlt");
	goto spin_lock;
}
