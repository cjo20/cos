#ifndef INCLUDE_FB_H
#define INCLUDE_FB_H
int fb_write(char * buf, unsigned int len);
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg);
void fb_clear();
void fb_backspace();
int test();
int strlen(char * s);
#endif