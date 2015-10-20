#include "fb.h"
#include "serial.h"
#include "gdt.h"
#include "interrupt.h"
#include "pic.h"
#include "keyboard.h"
#include "timer.h"

#include "multiboot.h"
#include "lib.h"
typedef void (*call_module_t)(void);


int kmain(unsigned int ebx)
{

	int i = 0;

	ebx = ebx;
	gdt_install();	
	idt_install();	
	irq_install();

	IRQ_disable(2);
	IRQ_disable(1);


	IRQ_clear_mask(1);
	IRQ_clear_mask(0);

	irq_install_handler(1, keyboard_handler);
	irq_install_handler(0, timer_handler);
	asm("sti");

	fb_clear();
	test();	
	fb_writeString("Booted in 5");
	char c[1];

	for (i = 4 ; i >= 0; --i)
	{
		c[0] = (char) (i + 48);
		timer_wait(18);
		fb_backspace();
		fb_write(c, 1);
	}
	fb_writeString("\n");
	fb_writeString("Chris' basic OS\n");

	serial_set_up();
	serial_write("hello, world\n", 13);
	serial_write("how are you?\n", 13);

#if 1
	multiboot_info_t * mbinfo = (multiboot_info_t *) ebx;

	if(mbinfo->flags & 0x8)
	{
		char str[30];
		unsigned int j;
		fb_writeString("Modules struct is Valid\n");

		itoa(mbinfo->mods_count, str, 10);
		fb_writeString(str);
		fb_writeString(" modules loaded.\n");

		multiboot_module_t * mod;
		mod = (multiboot_module_t *) mbinfo->mods_addr;

		for (j = 0; j < mbinfo->mods_count; j++, mod++)
		{
			fb_writeString("Module start: ");
			itoa(mod->mod_start, str, 16);
			fb_writeString(str);

			fb_writeString("\nModule end: ");
			itoa(mod->mod_end, str, 16);
			fb_writeString(str);			

			fb_writeString("\nModule cmd: ");
			fb_writeString((char *)mod->string);
			fb_writeString("\nCalling module\n");

			call_module_t start_program = (call_module_t) mod->mod_start;
			asm("xchg %bx,%bx");
			start_program();			
		}
	}
	else
	{
		fb_write("Flags not valid\n", 15);
	}	
	#endif

	while(i)
	{
		i = i;
	}

	return 0;
}