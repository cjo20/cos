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

void fb_set_foreground(unsigned char foreground)
{
	fg = foreground;
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
	else if (c == '\b')
	{
		fb_backspace();
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
	cx = 0;
	cy = 0;
	fb_move_cursor(cx, cy);
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

	char prefix = 0;
	char prefix_char = 'x';
	char zerofill = 0;
	int  min_width = 0;

	unsigned char original_fg = fg;
	unsigned char original_bg = bg;

	while((ch = *(format++)))
	{
		p = 0;

		if  (ch == 0x1b)
		{
			int code = 0;
			ch = *(format++);
			ch = *(format++);
			code = 0;
			while(ch >= '0' && ch <= '9')
			{
				code *= 10;
				code += (ch - '0');
				ch = *(format++);
			}

			if (code == 0)
			{
				fg = original_fg;
				bg = original_bg;
			}

			if (code >= 30 && code <= 37)
			{
				fg = code - 30;
			}

			if (code >= 40 && code <= 47)
			{
				bg = code - 40;
			}

		}
	    else if(ch != '%')
	    {
	        fb_putch(ch);
	    }
	    else
	    {
	        ch = *(format++);

	        if (ch == 0x1b)
	        {
				ch = *(format++);

	        }
	        if (ch == '#')
	        {
	        	prefix = 1;
	        	ch = *(format++);
	        }

	        if (ch == '0')
	        {
	        	zerofill = 1;
	        	ch = *(format++);
	        }

	        if (ch >= '0' && ch <= '9')
	        {
	        	min_width = ch - '0';
	        	ch = *(format++);
	        }
	        
	        switch(ch)
	        {
	            case 'd':
	           	case 'u':
	             //print int
	            	i = (int) va_arg(args, int);
	            	itoa(i, buf, 10);
	            	p = buf;
	                break;

	           	case 'i':
	             //print int
	            	i = (int) va_arg(args, int);
	            	utoa(i, buf, 10);
	            	p = buf;
	                break;

	            case 'x':
	               	i = (int) va_arg(args, int);
	               	if (prefix)
	               	{
	               		prefix_char = 'x';
	               	}
	            	
	            	utoa(i, buf, 16);

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
	    	int output_length = strlen(p);
	    	int pad_length = min_width - output_length;

	    	if (pad_length > 0)
	    	{
	    		int pad_length = min_width - output_length;
	    		char pad_char = zerofill? '0' : ' ';

	    		if (zerofill && prefix)
	    		{
					fb_putch('0');
	    			fb_putch(prefix_char);
	    		}

	    		while (pad_length)
	    		{
	    			fb_putch(pad_char);
	    			pad_length--;
	    		}

	    		if (!zerofill && prefix)
	    		{
					fb_putch('0');
	    			fb_putch(prefix_char);
	    		}
	    	}
	    	else
	    	{
	    		if (prefix)
	    		{
	    			fb_putch('0');
	    			fb_putch(prefix_char);
	    		}
	    	}
	    	fb_writeString(p);
	    }
		
		prefix = 0; 
	    zerofill = 0;
	    min_width = 0;
	}

	fg = original_fg;
	bg = original_bg;
}
