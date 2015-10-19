#include "fb.h"
#include "serial.h"
#include "gdt.h"
#include "interrupt.h"
#include "pic.h"
int kmain()
{

	int i = 0;
	char c[2];

	gdt_install();	

	IRQ_disable(2);
	IRQ_disable(1);
	pic_remap(0x20, 0x28);
	idt_install();		

	IRQ_clear_mask(1);

/*	char d = IRQ_get_mask(2);
	
	asm("movb %0, %%al; xchg %%bx, %%bx;"
				:
				:"g" (d)
				: );	
*/	

	fb_clear();
	test();
	fb_write("hello, world\n", 13);
	fb_write("test\n", 5);
	
	//i /= (i - 0);

	c[1] = '\n';

	for (i = 0; i < 50; ++i)
	{
		c[0] = i + 65;
		fb_write(c, 2);
	}

	serial_set_up();
	serial_write("hello, world\n", 13);
	serial_write("how are you?\n", 13);

	while(i)
	{
		int u = pic_get_irr();
		//int v = pic_get_isr();
		if (u)
		{
			//asm("movl %0, %%eax; movl %1, %%ecx; xchg %%bx, %%bx;"
			//	:
			//	:"g" (u), "g" (v) 
			//	: );
		}//
		i = i;
	}
	

	return 0;
}