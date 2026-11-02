#ifndef TEXT_H
#define TEXT_H

#include "def.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

#define WHITE_TEXT 0x0F
#define MAX_DIGITS 10

#define TEXT_BUF_ADDR 0xB8000

void putc(u32 y, u32 x, u8 c, u8 attributes);
void __attribute__((stdcall)) puts(u32 y, u32 x, u8* str, u8 attributes);
void __attribute__((stdcall)) putnum(u32 y, u32 x, u32 num, u8 attributes);

#endif
