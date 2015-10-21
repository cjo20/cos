#include "fb.h"
#include "serial.h"
#include "gdt.h"
#include "interrupt.h"
#include "pic.h"
#include "keyboard.h"
#include "timer.h"
#include "asm_func.h"
#include "multiboot.h"
#include "lib.h"
#include "cpuinfo.h"
#define BUILDSTR __DATE__  " "  __TIME__  "\n"
typedef void (*call_module_t)(void);



int kmain(int virt_start, int virt_end, int phy_start, int phy_end, unsigned int ebx)
{
	//char string[32];
	int i = 0;
	gdt_install();	
	idt_install();	
	irq_install();
	
	IRQ_disable(2);
	IRQ_disable(1);


	IRQ_clear_mask(1);
	IRQ_clear_mask(0);

	irq_install_handler(1, keyboard_handler);
	irq_install_handler(0, timer_handler);
	irq_install_handler(7, irq_spurious_handler);
	irq_install_handler(15, irq_spurious_handler);
	pic_acknowledge(10);
	asm("sti");

	fb_clear();
	test();	
	printf("Timer Test:5");
	char c[1];

	for (i = 4 ; i >= 0; --i)
	{
		c[0] = (char) (i + 48);
		timer_wait(18);
		fb_backspace();
		fb_write(c, 1);
	}
	printf("\n");

	serial_set_up();
	serial_write("hello, world\n", 13);
	serial_write("how are you?\n", 13);

	int cpu_id[4];

	get_cpuid(0, cpu_id);
	print_vendor_string(cpu_id);

	get_cpuid(1, cpu_id);
	parse_cpuid_features(cpu_id);

	printf("\n%x:%x\n", cpu_id[2], cpu_id[3]);

//	asm("int $0x20");

	printf("Chris' basic OS. Built: %s", BUILDSTR);
	
#if 0

	
	itoa(phy_start, string, 16);
	fb_writeString("Physical address start: ");
	fb_writeString(string);
	itoa(phy_end, string, 16);
	fb_writeString("\nPhysical address end: ");
	fb_writeString(string);	
	itoa(virt_start, string, 16);
	fb_writeString("\nVirtual address start: ");
	fb_writeString(string);
	itoa(virt_end, string, 16);
	fb_writeString("\nVirtual address end: ");
	fb_writeString(string);
	fb_writeString("\n");	
#else
	virt_start = virt_start;
	virt_end = virt_end;
	phy_start = phy_start;
	phy_end = phy_end;
#endif

#if 1

	multiboot_info_t * mbinfo = (multiboot_info_t *) (ebx + 0xC0000000);

	if(mbinfo->flags & 0x8)
	{
		unsigned int j;
		printf("Modules struct is valid\n");
		printf("%d modules loaded.\n", mbinfo->mods_count);

		multiboot_module_t * mod;
		mod = (multiboot_module_t *) (mbinfo->mods_addr + 0xC0000000);

		for (j = 0; j < mbinfo->mods_count; j++, mod++)
		{
			mod->mod_start += 0xC0000000;
			mod->mod_end += 0xC0000000;
			mod->string += 0xC0000000;
			printf("Module start:\t%x\n", mod->mod_start);
			printf("Module end:\t\t%x\n", mod->mod_end);
			printf("Module cmd:\t\t%s\n", (char *)(mod->string));
			printf("Calling module\n");

			call_module_t start_program = (call_module_t) (mod->mod_start);
			start_program();			
		}
	}
	else
	{
		printf("Modules not valid\n");
	}	
#else
	ebx = ebx;

#endif

	while(i)
	{
		i = i;
	}

	return 0;
}