#include "io.h"
#include "mem.h"
#include "lib.h"
#include "fb.h"
#include "stdarg.h"
/* The I/O ports */
#define FB_COMMAND_PORT		0x3D4
#define FB_DATA_PORT		0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND 	14
#define FB_LOW_BYTE_COMMAND		15


#define TAB_WIDTH 4

static char * fb = (char *)0xC00B8000;
static unsigned int cx = 0, cy = 0;
static unsigned char fg = 7, bg = 0;

/** fb_move_cursor:
*	Moves the cursor of the framebuffer to the given position
*
* @param pos The new position of the cursor
*/
void fb_move_cursor(unsigned short x, unsigned short y)
{
	unsigned short pos = (80 * y) + x;
	outb(FB_COMMAND_PORT,	FB_HIGH_BYTE_COMMAND);
	outb(FB_DATA_PORT,		((pos >> 8) & 0xFF));
	outb(FB_COMMAND_PORT, 	FB_LOW_BYTE_COMMAND);
	outb(FB_DATA_PORT, 		pos & 0xFF);
}

void fb_set_colour(unsigned char foreground, unsigned char background)
{
	fg = foreground;
	bg = background;
}

void newline()
{
	cx = 0;
	cy++;

	if (cy > 24)
	{
		int line = 0;
		for (line = 0; line < 25; ++line)
		{
			memcpy(fb + 160*line, fb + 160*(line + 1), 160);
		}
		cy = 24;
	}

	fb_move_cursor(cx, cy);

}


void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{
	fb[2*i] = c;
	fb[2*i+1] = ((bg & 0x0F) << 4) | (fg & 0x0F);
}

void fb_putch(char c)
{
	int position = (80 * cy) + cx;

	if (c == '\n')
	{
		newline();
		fb_move_cursor(cx, cy);
		return;
	}
	else if (c == '\t')
	{
		int spaces = TAB_WIDTH - (cx % TAB_WIDTH);
		int sp = 0;
		for (sp = 0; sp < spaces; sp++)
		{
			fb_putch(' ');
		}
	}
	else
	{
		fb_write_cell(position, c, fg, bg);
		cx++;
	}
	
	

	if (cx >= 80)
	{
		newline();
	}

	fb_move_cursor(cx, cy);
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

	for (i = 0; i < len; ++i)
	{
		char c = *(buf + i);
		fb_putch(c);
	}
	
	return len;
}

void fb_backspace()
{
	if (cx > 0 || cy > 0)
	{
		if (cx == 0)
		{
			cy--;
			cx = 79;
		}
		else
		{
			cx--;
		}
		
		fb_write_cell((80*cy)+cx, ' ', 7, 0);
		fb_move_cursor(cx, cy);
	}
}


int fb_writeString(char * buf)
{
	return fb_write(buf, strlen(buf));
}

void test()
{
	int i = 0;
	fb_writeString("Colour test:");
	for (i = 0; i < 15; i++)
	{
		fb_set_colour(i, 0);
		fb_putch('A');
	}

	for (i = 0; i < 15; i++)
	{
		fb_set_colour(0, i);
		fb_putch('A');
	}	

	fb_set_colour(7, 0);
	printf("\nCharacter test:");
	unsigned char c = 0;
	for (c = 0; c < 255; c++)
	{
		fb_putch(c);
	}
	printf("\n");
}

void printf(char * format, ...)
{
	va_list args;

	va_start(args, format);
	char ch;
	char * p = 0;
	char c;
	char buf[33];
	int i;

	while((ch = *(format++)))
	{
		p = 0;

	    if(ch != '%')
	    {
	        fb_putch(ch);
	    }
	    else
	    {
	        ch = *(format++);

	        switch(ch)
	        {
	            case 'd':
	             //print int
	            	i = (int) va_arg(args, int);
	            	itoa(i, buf, 10);
	            	p = buf;
	                break;

	            case 'x':
	               	i = (int) va_arg(args, int);
	               	buf[0] = '0';
	               	buf[1] = 'x';
	            	utoa(i, buf + 2, 16);
	            	p = buf;
	                break;

	            case 'c':
	                c = (char) va_arg(args, unsigned int);
	                fb_putch(c);
	            break;

	            case 's':
	            	p = (char *) va_arg(args, char *);
	            break;

	        }
	    }

	    if(p)
	    {
	    	fb_writeString(p);
	    }
	}
}
