#include "io.h"
#include "mem.h"
/* The I/O ports */
#define FB_COMMAND_PORT		0x3D4
#define FB_DATA_PORT		0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND 	14
#define FB_LOW_BYTE_COMMAND		15

static char * fb = (char *)0x000B8000;
static unsigned int position = 0;
static const unsigned int max_position = 25 * 80;
static unsigned short current_cursor = 0;


/** fb_move_cursor:
*	Moves the cursor of the framebuffer to the given position
*
* @param pos The new position of the cursor
*/
void fb_move_cursor(unsigned short pos)
{
	outb(FB_COMMAND_PORT,	FB_HIGH_BYTE_COMMAND);
	outb(FB_DATA_PORT,		((pos >> 8) & 0xFF));
	outb(FB_COMMAND_PORT, 	FB_LOW_BYTE_COMMAND);
	outb(FB_DATA_PORT, 		pos & 0xFF);
	current_cursor = pos;
}

void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{
	fb[2*i] = c;
	fb[2*i+1] = ((bg & 0x0F) << 4) | (fg & 0x0F);
}

void fb_clear()
{
	int i =0;
	for (i = 0; i < 80*25; ++i)
	{
		fb_write_cell(i, ' ', 0, 0);
	}
}

int fb_write(char * buf, unsigned int len)
{	
	unsigned int i = 0;
	unsigned int new_position = position;
	char non_printable = 0;

	for (i = 0; i < len; ++i)
	{
		char c = *(buf + i);

		if (c == '\n')
		{
			new_position += 80;
			new_position -= (position % 80);
			non_printable = 1;
		}
		else
		{
			new_position += 1;
		}

		if (new_position >= max_position)
		{
			//Scroll!
			new_position -= 80;
			int line = 0;
			for (line = 0; line < 25; ++line)
			{
				memcpy(fb + 160*line, fb + 160*(line + 1), 160);
			}
			
		}

		if (!non_printable)
		{
			fb_write_cell(new_position - 1, *(buf+i), 7, 0);
		}

		position = new_position;
	}
	fb_move_cursor(position);
	
	return len;
}


void test()
{
	//int position = 80*19;
	int i = 0;
	for (i = 0; i < 15; i++)
	{
		fb_write_cell(position, 'A', i, 0);
		fb_write("\n", 1);
	}

	for (i = 0; i < 15; i++)
	{
		fb_write_cell(position,  'A', 0, i);
		fb_write("\n", 1);
	}	
}


void fb_backspace()
{
	if (position > 0)
	{
		position--;
		fb_write_cell(position, ' ', 7, 0);
		fb_move_cursor(position + 1);
	}
}

int strlen(char * s)
{
	int i = 0;

	while(*s != 0)
	{
		i++;
		s++;
	}

	return i;
}