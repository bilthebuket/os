#include "shared_include/text.h"
#include "kernel_include/global.h"
#include "shared_include/mem.h"
#include "kernel_include/interrupts.h"
#include "kernel_include/disk.h"
#include "shared_include/disk.h"
#include "kernel_include/file.h"

void _start(u32 root_lba, u32 __drive_mask)
{
	clear_screen();
	drive_mask = __drive_mask;
	init_root_lba(root_lba);
	u32 root_directory_lba = find_available_sectors(SECTOR_SIZE);
	if (!root_directory_lba)
	{
		puts(0,0,"couldnt find a sector to put the root directory",WHITE_TEXT);
		asm("hlt");
	}
	init_root_directory(root_directory_lba);
	setup_interrupts(KERNEL_INFO_VIRTUAL_ADDRESS + IDT_ADDRESS);
	puts(0,0,"reached kernel main",WHITE_TEXT);
	/*
	file_create(&root_directory, "file", 0);
	File f;
	file_open(&f, &root_directory, "file");
	const char str[] = "some text";
	file_write(&f, (u8*) &str[0], sizeof(str));
	u8 buf[sizeof(str)];
	file_read(&f, buf, sizeof(buf));
	puts(1,0,buf,WHITE_TEXT);
	*/
spin_lock:
	asm("hlt");
	goto spin_lock;
}
