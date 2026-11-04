#include "string.h"

int strcmp(const u8* one, const u8* two)
{
	int i = 0;
	for (; one[i] != '\0' && two[i] != '\0'; i++)
	{
		if (one[i] != two[i])
		{
			break;
		}
	}

	if (one[i] == '\0' && two[i] == '\0')
	{
		return 0;
	}
	else if (one[i] == '\0' && two[i] != '\0')
	{
		return STR2_BIGGER;
	}
	else if (one[i] != '\0' && two[i] == '\0')
	{
		return STR1_BIGGER;
	}
	return SAME_LENGTH_UNEQUAL;
}
