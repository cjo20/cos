#include "io.h"
#include "fb.h"
#include "interrupt.h"
#include "keyboard.h"
#include "stdint.h"
/* KBDUS means US Keyboard Layout. This is a scancode table
*  used to layout a standard US keyboard. I have left some
*  comments in to give you an idea of what key is what, even
*  though I set it's array index to 0. You can change that to
*  whatever you want using a macro, if you wish! */

static KEYCODE keymap[128] =
{
  KEY_UNKNOWN,
  KEY_ESCAPE,
  KEY_1,
  KEY_2,
  KEY_3,
  KEY_4,
  KEY_5,
  KEY_6,
  KEY_7,
  KEY_8,
  KEY_9,
  KEY_0,
  KEY_MINUS,
  KEY_EQUAL,
  KEY_BACKSPACE,
  KEY_TAB,
  KEY_Q,
  KEY_W,
  KEY_E,
  KEY_R,
  KEY_T,
  KEY_Y,
  KEY_U,
  KEY_I,
  KEY_O,
  KEY_P,
  KEY_LEFTBRACKET,
  KEY_RIGHTBRACKET,
  KEY_RETURN,
  KEY_UNKNOWN,
  KEY_A,
  KEY_S,
  KEY_D,
  KEY_F,
  KEY_G,
  KEY_H,
  KEY_J,
  KEY_K,
  KEY_L,
  KEY_SEMICOLON,
  KEY_QUOTE,
  KEY_GRAVE,
  KEY_LSHIFT,
  KEY_BACKSLASH,
  KEY_Z,
  KEY_X,
  KEY_C,
  KEY_V,
  KEY_B,
  KEY_N,
  KEY_M,
  KEY_COMMA,
  KEY_DOT,
  KEY_SLASH,
  KEY_RSHIFT,
  KEY_UNKNOWN,
  KEY_UNKNOWN,
  KEY_SPACE,
  KEY_CAPSLOCK,
  KEY_F1,
  KEY_F2,
  KEY_F3,
  KEY_F4,
  KEY_F5,
  KEY_F6,
  KEY_F7,
  KEY_F8,
  KEY_F9,
  KEY_F10,
  KEY_NUMLOCK,
  KEY_SCROLL_LOCK,
  KEY_UP,
  KEY_PGUP,
  KEY_MINUS,
  KEY_LEFT,
  KEY_UNKNOWN,
  KEY_RIGHT,
  KEY_PLUS,
  KEY_END,
  KEY_DOWN,
  KEY_INSERT,
  KEY_DELETE,
  KEY_UNKNOWN,
  KEY_UNKNOWN,
  KEY_UNKNOWN,
  KEY_F11,
  KEY_F12,
  KEY_UNKNOWN
};

KEYCODE current_key[32] = { KEY_UNKNOWN };
uint8_t key_index = 0;
uint8_t key_head = 0;

int capslock_on = 0;

char shift_on = 0;

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
	unsigned int is_extended = 0;
	KEYCODE mapped_key;
	r=r;
	scancode = inb(0x60);

	if (scancode == 0xE1)
	{
		inb(0x60);
		inb(0x60);
		return;
	}
	else if (scancode == 0xE0)
	{
		scancode = inb(0x60);
		is_extended = 1;
	}

	if ((key_index+1) == key_head)
	{
		return;
	}

	if (scancode & 0x80)
	{
		scancode &= 0x7F;
		mapped_key = (scancode < 87)?keymap[scancode]:KEY_UNKNOWN;

		if (!is_extended && (mapped_key == KEY_LSHIFT || mapped_key == KEY_RSHIFT))
		{
			shift_on = 0;
		}
	}
	else
	{
		if (is_extended)
		{
			switch(scancode)
			{
				case 0x48: mapped_key = KEY_UP; break; 
				case 0x4B: mapped_key = KEY_LEFT; break;
				case 0x50: mapped_key = KEY_DOWN; break;
				case 0x4D: mapped_key = KEY_RIGHT; break;
		  		default: mapped_key = KEY_UNKNOWN; break;
			}
			current_key[key_index++] = mapped_key;
			is_extended = 0;
	  	}
	  	else
	  	{
	  		mapped_key = (scancode < 87)?keymap[scancode]:KEY_UNKNOWN;

			if (!is_extended && (mapped_key == KEY_LSHIFT || mapped_key == KEY_RSHIFT))
			{
				shift_on = 1;
			}
			else if (mapped_key == KEY_CAPSLOCK)
			{
				capslock_on = 1 - capslock_on;
				outb(0x60, 0xED);
				outb(0x60, (capslock_on) << 2);
			}
			else
			{
				current_key[key_index++] = mapped_key;
			}
	  	}
		//	  switch(scancode)
		//	  {
		//		case 42:
		//		case 54:
		//		  shift_held = 1;
		//		  break;
		//		case 14: //backspace
		//		  fb_backspace();
		//		  break;
		//		default:
		//		  if (shift_held )
		//		  {
		//			fb_write(&shifted[(int) keymap[scancode]], 1);
		//		  }
		//		  else
		//		  {
		//			char c = keymap[scancode];
		//			fb_write(&c, 1);
		//		  }
		//		  break;

		//	  }
	  
	}

	key_index &= 0x1F;
}

KEYCODE get_key(void)
{
	if (key_index == key_head)
	{
		return KEY_UNKNOWN;
	}

	KEYCODE key = current_key[key_head];
	current_key[key_head] = KEY_UNKNOWN;

	key_head++;
	key_head &= 0x1F;
	return key;
}

int get_SHIFT_stat(void)
{
	return shift_on;
}

int get_CAPSLOCK_stat()
{
	return capslock_on;
}

char getc()
{
	int shift_on;
	int capslock_on;

	KEYCODE key = get_key();

	while (key == KEY_UNKNOWN
		|| key == KEY_LSHIFT
		|| key == KEY_RSHIFT)
	{
		key = get_key();
	}

	shift_on = get_SHIFT_stat();
	capslock_on = get_CAPSLOCK_stat();

	if (key >= 'a' && key <= 'z')
	{
		if (shift_on || capslock_on)
		{
			key -= 32;
		}
	}
	else if (shift_on)
	{
		switch (key)
		{
			case '0': key  = ')'; break;
			case '1': key  = '!'; break;
			case '2': key  = '"'; break;
			case '3': key  = '#'; break;
			case '4': key  = '$'; break;
			case '5': key  = '%'; break;
			case '6': key  = '^'; break;
			case '7': key  = '&'; break;
			case '8': key  = '*'; break;
			case '9': key  = '('; break;

			case '`': key  = '~'; break;
			case '-': key  = '_'; break;
			case '=': key  = '+'; break;
			case '[': key  = '{'; break;
			case ']': key  = '}'; break;
			case '\\': key  = '|'; break;
			case ';': key  = ':'; break;
			case '\'': key  = '"'; break;
			case ',': key  = '<'; break;
			case '.': key  = '>'; break;
			case '/': key  = '?'; break;
			default: break;
		}
	}

	return (char) key;
}

void init_keyboard()
{
	//current_key = KEY_UNKNOWN;
	shift_on = 0;
	capslock_on = 0;
	outb(0x60, 0xED);
	outb(0x60, 0x00);
}
//char kbdus[128] =
//{
//    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
//  '9', '0', '-', '=', '\b', /* Backspace */
//  '\t',     /* Tab */
//  'q', 'w', 'e', 'r', /* 19 */
//  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
//    0,      /* 29   - Control */
//  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
// '\'', '`',   0,    /* Left shift */
// '\\', 'z', 'x', 'c', 'v', 'b', 'n',      /* 49 */
//  'm', ',', '.', '/',   0,        /* Right shift */
//  '*',
//    0,  /* Alt */
//  ' ',  /* Space bar */
//    0,  /* Caps lock */
//    0,  /* 59 - F1 key ... > */
//    0,   0,   0,   0,   0,   0,   0,   0,
//    0,  /* < ... F10 */
//    0,  /* 69 - Num lock*/
//    0,  /* Scroll Lock */
//    0,  /* Home key */
//    0,  /* Up Arrow */
//    0,  /* Page Up */
//  '-',
//    0,  /* Left Arrow */
//    0,
//    0,  /* Right Arrow */
//  '+',
//    0,  /* 79 - End key*/
//    0,  /* Down Arrow */
//    0,  /* Page Down */
//    0,  /* Insert Key */
//    0,  /* Delete Key */
//    0,   0,   0,
//    0,  /* F11 Key */
//    0,  /* F12 Key */
//    0,  /* All other keys are undefined */
//};  
