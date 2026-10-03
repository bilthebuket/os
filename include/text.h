#ifndef TEXT_H
#define TEXT_H

#include "def.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

#define WHITE_TEXT 0x0F

void putc(u32 y, u32 x, u8 c, u8 attributes);

#endif
