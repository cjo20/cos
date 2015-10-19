#include "io.h"
#include "fb.h"
#include "interrupt.h"
/* KBDUS means US Keyboard Layout. This is a scancode table
*  used to layout a standard US keyboard. I have left some
*  comments in to give you an idea of what key is what, even
*  though I set it's array index to 0. You can change that to
*  whatever you want using a macro, if you wish! */
char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};		

char shift_held = 0;

char shifted[128] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
          /*20 */     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '1', '2', '~', '4', '5', \
                      '7', '@', '9', '0', '8', '=', '<', '_', '>', '?', ')', '!', '\"', '#', '$', \
                      '%', '^', '&', '*', '(', ';', ':', ',', '+', '.', '/', '\'', 'a', 'b', 'c', \
                      'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', \
                      't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}',  '6', '-', ' ', 'A', 'B', 'C', 'D', \
                      'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', \
                      'U', 'V', 'W', 'X', 'Y', 'Z', '[', ']', '#', 0};


void keyboard_handler(struct cpu_state * r)
{
    unsigned int scancode;
    r=r;
    scancode = inb(0x60);

    if (scancode & 0x80)
    {

      switch (scancode ^ 0x80)
      {
        case 42:
        case 54:
          shift_held = 0;
          break;
      }
    }
    else
    {
      switch(scancode)
      {
        case 42:
        case 54:
          shift_held = 1;
          break;
        case 14: //backspace
          fb_backspace();
          break;
        default:
          if (shift_held )
          {
            fb_write(&shifted[(int) kbdus[scancode]], 1);
          }
          else
          {
            fb_write(&kbdus[scancode], 1);
          }
          break;

      }
      
    }
}