#ifndef TEXT_H
#define TEXT_H

#include "def.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

#define WHITE_TEXT 0x0F
#define MAX_DIGITS 10

void putc(u32 y, u32 x, u8 c, u8 attributes);
void puts(u32 y, u32 x, u8* str, u8 attributes);
void putnum(u32 y, u32 x, u32 num, u8 attributes);

#endif
