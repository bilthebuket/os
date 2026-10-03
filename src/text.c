#include "text.h"
#include "def.h"

#define TEXT_BUF_ADDR 0xB8000
#define BYTES_PER_CELL 2

void putc(u32 y, u32 x, u8 c, u8 attributes)
{
	char* buf = (char*) TEXT_BUF_ADDR;
	if (y >= SCREEN_HEIGHT || x >= SCREEN_WIDTH)
	{
		return;
	}

	buf[y * BYTES_PER_CELL + x] = c;
	buf[y * BYTES_PER_CELL + x + 1] = attributes;
}
