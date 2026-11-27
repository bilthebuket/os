#ifndef KERNEL_STRING_H
#define KERNEL_STRING_H

#include "shared_include/def.h"

#define STR2_BIGGER 1
#define STR1_BIGGER 2
#define SAME_LENGTH_UNEQUAL 3

#define NUMBER_MAX_DIGITS 11

int strcmp(const u8* one, const u8* two);

#endif
