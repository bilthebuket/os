#include "text.h"
#include "def.h"
#include "paging.h"
#include "mem.h"

#define BYTES_PER_CELL 2

void putc(u8* text_buf_addr, u32 y, u32 x, u8 c, u8 attributes)
{
	if (!paging_enabled_huh())
	{
		text_buf_addr = (void*) TEXT_BUF_ADDR;
	}

	if (y >= SCREEN_HEIGHT || x >= SCREEN_WIDTH)
	{
		return;
	}

	text_buf_addr[y * SCREEN_WIDTH * BYTES_PER_CELL + x * BYTES_PER_CELL] = c;
	text_buf_addr[y * SCREEN_WIDTH * BYTES_PER_CELL + x * BYTES_PER_CELL + 1] = attributes;
}

void __attribute__((stdcall)) puts(u32 text_buf_addr, u32 y, u32 x, u8* str, u8 attributes)
{
	for (u32 i = 0; str[i] != '\0'; i++)
	{
		putc((void*) text_buf_addr, y, x + i, str[i], attributes);
	}
}

void __attribute__((stdcall)) putnum(u32 text_buf_addr, u32 y, u32 x, u32 num, u8 attributes)
{
	u8 buf[MAX_DIGITS];
	for (u8 i = 0; i < MAX_DIGITS; i++)
	{
		buf[MAX_DIGITS - i - 1] = '0' + num % 10;
		num /= 10;
	}
	u8* str = &buf[0];
	u8 i = 0;
	for (; i < MAX_DIGITS && str[0] == 0; i++, str = &str[1]) {}
	for (u8 j = 0; i < MAX_DIGITS; i++, j++)
	{
		putc((void*) text_buf_addr, y, x + j, str[j], attributes);
	}
}
