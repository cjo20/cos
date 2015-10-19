#include "fb.h"
#include "serial.h"
#include "gdt.h"
#include "interrupt.h"
#include "pic.h"
#include "keyboard.h"

int kmain()
{

	int i = 0;
//	char c[2];

	gdt_install();	
	idt_install();	
	irq_install();

	IRQ_disable(2);
	IRQ_disable(1);


	IRQ_clear_mask(1);
	irq_install_handler(1, keyboard_handler);
	asm("sti");

	test();
	fb_write("hello, world\n", 13);
	fb_write("test\n", 5);
	
	//i /= (i - 0);

//	c[1] = '\n';
//
//	for (i = 0; i < 50; ++i)
//	{
//		c[0] = i + 65;
//		fb_write(c, 2);
//	}

	serial_set_up();
	serial_write("hello, world\n", 13);
	serial_write("how are you?\n", 13);

	while(i)
	{
		i = i;
	}

	return 0;
}