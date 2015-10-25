#include "mem.h"
#include "fb.h"
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


void memsetd(void * destination, uint32_t val, unsigned int size)
{
	uint32_t * dest = (uint32_t *) destination;
	unsigned int i = 0;

	for (i = 0; i < size; ++i)
	{
		*dest = val;
		dest++;
	}	
}

void memset(void * destination, char val, unsigned int size)
{
	char * dest = (char *) destination;
	unsigned int i = 0;

	for (i = 0; i < size; ++i)
	{
		*dest = val;
		dest++;
	}
}