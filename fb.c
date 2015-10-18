#include "io.h"

/* The I/O ports */
#define FB_COMMAND_PORT		0x3D4
#define FB_DATA_PORT		0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND 	14
#define FB_LOW_BYTE_COMMAND		15

char * fb = (char *)0x000B8000;
/** fb_move_cursor:
*	Moves the cursor of the framebuffer to the given position
*
* @param pos The new position of the cursor
*/
static void fb_move_cursor(unsigned short pos)
{
	outb(FB_COMMAND_PORT,	FB_HIGH_BYTE_COMMAND);
	outb(FB_DATA_PORT,		((pos >> 8) & 0x00FF));
	outb(FB_COMMAND_PORT, 	FB_LOW_BYTE_COMMAND);
	outb(FB_DATA_PORT, 		pos & 0x00FF);
}

static void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{
	fb[2*i] = c;
	fb[2*i+1] = ((bg & 0x0F) << 4) | (fg & 0x0F);
}

void test()
{
	int position = 80*19;
	int i = 0;
	for (i = 0; i < 15; ++i)
	{
		fb_write_cell(position + i, 'A', i, 0);
	}

	position = 80*20;
	for (i = 0; i < 15; ++i)
	{
		fb_write_cell(position + i, 'A', 0, i);
	}	
}
int write(char * buf, unsigned int len)
{
	static int position = 0;
	const int max_position = 25 * 80;
	
	unsigned int i = 0;
	for (i = 0; i < len; ++i)
	{
		char c = *(buf + i);

		if (c == '\n')
		{
			position += 80;
			position -= position % 80;
		}
		else
		{
			fb_write_cell(position++, *(buf+i), 7, 0);
			position %= max_position;
		}
	}
	fb_move_cursor(position);
	
	return len;
}