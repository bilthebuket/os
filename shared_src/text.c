#include "shared_include/text.h"
#include "shared_include/def.h"
#include "shared_include/paging.h"
#include "shared_include/mem.h"
#include "shared_include/string.h"

#define BYTES_PER_CELL 2

void putc(u32 y, u32 x, u8 c, u8 attributes)
{
	u8* text_buf_addr;
	if (!paging_enabled_huh())
	{
		text_buf_addr = (void*) TEXT_BUF_ADDR;
	}
	else
	{
		text_buf_addr = (void*) FRAME_ZERO_VIRTUAL_ADDRESS + TEXT_BUF_ADDR;
	}

	if (y >= SCREEN_HEIGHT || x >= SCREEN_WIDTH)
	{
		return;
	}

	text_buf_addr[y * SCREEN_WIDTH * BYTES_PER_CELL + x * BYTES_PER_CELL] = c;
	text_buf_addr[y * SCREEN_WIDTH * BYTES_PER_CELL + x * BYTES_PER_CELL + 1] = attributes;
}

void __attribute__((stdcall)) puts(u32 y, u32 x, u8* str, u8 attributes)
{
	for (u32 i = 0; str[i] != '\0'; i++)
	{
		putc(y, x + i, str[i], attributes);
	}
}

void __attribute__((stdcall)) putnum(u32 y, u32 x, u32 num, u8 attributes)
{
	u8 buf[NUMBER_MAX_DIGITS];
	int_to_str(num, buf);
	puts(y, x, buf, attributes);
}

void clear_screen(void)
{
	for (u32 i = 0; i < SCREEN_HEIGHT; i++)
	{
		for (u32 j = 0; j < SCREEN_WIDTH; j++)
		{
			putc(i, j, ' ', WHITE_TEXT);
		}
	}
}
