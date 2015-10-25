#ifndef INCLUDE_FB_H
#define INCLUDE_FB_H
int fb_writeString(char * buf);
int fb_write(char * buf, unsigned int len);
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg);
void fb_putch(char c);
void fb_clear();
void fb_backspace();
void test();
void printf(char * format, ...);
void fb_set_colour(unsigned char foreground, unsigned char background);
void fb_set_vid_mem(char * new_fb);

#define FG_COLOUR_NRM 		"\x1B[0m"
#define FG_COLOUR_BLACK 	"\x1B[31m"
#define FG_COLOUR_BLUE 		"\x1B[31m"
#define FG_COLOUR_GREEN 	"\x1b[32m"
#define FG_COLOUR_AQUA		"\x1b[33m"
#define FG_COLOUR_RED 		"\x1b[34m"
#define FG_COLOUR_PURPLE 	"\x1b[35m"
#define FG_COLOUR_ORANGE	"\x1b[36m"
#define FG_COLOUR_WHITE		"\x1b[37m"
#define KRST 				"\x1b[0m"
#endif