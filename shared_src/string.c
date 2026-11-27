#include "shared_include/string.h"

void int_to_str(u32 num, u8* buf)
{
	for (u8 i = 0; i < NUMBER_MAX_DIGITS; i++)
	{
		buf[NUMBER_MAX_DIGITS - i - 1] = '0' + num % 10;
		num /= 10;
	}
	u32 to_remove = 0;
	for (; to_remove < NUMBER_MAX_DIGITS && buf[to_remove] == '0'; to_remove++) {}
	for (u32 i = 0; i < NUMBER_MAX_DIGITS - to_remove; i++)
	{
		buf[i] = buf[i + to_remove];
	}
	buf[NUMBER_MAX_DIGITS - to_remove] = '\0';
}
