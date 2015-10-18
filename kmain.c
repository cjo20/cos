#include "fb.h"

int kmain()
{
	int i = 0;
	char c[2];

	fb_clear();
	test();
	write("hello, world\n", 13);
	write("test\n", 5);
	
	c[1] = '\n';

	for (i = 0; i < 26; ++i)
	{
		c[0] = i + 65;
		write(c, 2);
	}

	return 0;
}