#include "fb.h"
#include "serial.h"

int kmain()
{
	int i = 0;
	char c[2];

	fb_clear();
	test();
	fb_write("hello, world\n", 13);
	fb_write("test\n", 5);
	
	c[1] = '\n';

	for (i = 0; i < 50; ++i)
	{
		c[0] = i + 65;
		fb_write(c, 2);
	}

	serial_set_up();
	serial_write("hello, world\n", 13);
	serial_write("how are you?\n", 13);
	return 0;
}