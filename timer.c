#include "io.h"
#include "fb.h"
#include "interrupt.h"

#define COMMAND_PORT 	0x43
#define TIMER_PORT 		0x40

/* command byte format 
* 	| 7  6 | 5 4 | 3 2 1 | 0   |
*	| CNTR | RW  | Mode  | BCD |
*
* CNTR selects which counter to use.
* 0 = Timer
* 1 = Undefined
* 2 = Speaker
*
* RW is writing mode.
* 1 = LSB
* 2 = MSB
* 3 = LSB then MSB
*
* Mode is timer mode
* 0 = Interrupt on terminal count
* 1 = Hardware retriggerable one shot
* 2 = Rate Generator
* 3 = Square Wave Mode
* 4 = Software Strobe
* 5 = Hardware Strobe
*/

void timer_phase(int hz)
{
	int divisor = 1193180 / hz;

	/*
	* CNTR = 0
	* RW = 3
	* Mode = 3
	* BCD = 0
	*/
	outb(COMMAND_PORT, 0x36);
	outb(TIMER_PORT, divisor & 0xFF);
	outb(TIMER_PORT, (divisor >> 8) & 0xFF);
}

unsigned int timer_ticks = 0;

void timer_handler(struct cpu_state * r)
{
	static char even = 0;
	r = r;
	timer_ticks++;

	if (timer_ticks % 18 == 0)
	{
		if (even)
		{
			fb_write_cell(79, '#', 7, 0);
		}
		else
		{
			fb_write_cell(79, ' ', 7, 0);
		}

		even++;
		even %= 2;
		//char * msg = "One second has passed\n";
		//fb_write(msg, strlen(msg));
	}
}

void timer_wait(unsigned int ticks)
{
	unsigned long eticks;
	eticks = timer_ticks + ticks;
	while(timer_ticks < eticks);
}