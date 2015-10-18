#include "mem.h"

void * memcpy(void * destination, const void * source, unsigned int num)
{
	unsigned int i = 0;
	char * dest = (char *) destination;
	const char * src = (char *) source;

	for (i = 0; i < num; ++i)
	{
		*(dest + i) = *(src + i);
	}

	return destination;
}