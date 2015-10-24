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
#include "mmngr_phys.h"
#include "mmngr_virtual.h"
#include "console.h"

#define DEBUG 0
#define BUILDSTR __DATE__  " "  __TIME__  "\n"
typedef void (*call_module_t)(void);
extern char * fb;

void enable_handlers()
{
	IRQ_disable(2);
	IRQ_disable(1);
	IRQ_clear_mask(1);
	IRQ_clear_mask(0);
	
	init_keyboard();
	irq_install_handler(1, keyboard_handler);
	irq_install_handler(0, timer_handler);
	irq_install_handler(7, irq_spurious_handler);
	irq_install_handler(15, irq_spurious_handler);
	pic_acknowledge(10);


	asm("sti");
}

void init_memory(unsigned int location, unsigned int mem_size, multiboot_info_t * mbinfo)
{
	if (mbinfo->flags & MULTIBOOT_FLAGS_MEMINFO)
	{
		mem_size = mbinfo->mem_upper + mbinfo->mem_lower + 1024;
	}
	else
	{
		printf("\t\t\t\t\t\t\t\t\t\t"FG_COLOUR_RED"[FAIL]\n");
		asm("hlt");
	}

	pmmngr_init(mem_size, (uint32_t *) location);
	printf("Initialised memory manager to handle %d kB", mem_size);

	printf("\t\t\t"FG_COLOUR_GREEN"[OK]\n");
	printf("\tMarking unused regions:");

	if (mbinfo->flags & MULTIBOOT_FLAGS_MMAP)
	{
		multiboot_memory_map_t * mmap = 0;
		multiboot_memory_map_t * mmap_end = 0;
		
		mbinfo->mmap_addr += 0xC0000000;

		mmap = (multiboot_memory_map_t *) mbinfo->mmap_addr;
		mmap_end = (multiboot_memory_map_t *) (mbinfo->mmap_addr + mbinfo->mmap_length);


		while (mmap < mmap_end)
		{
			#if DEBUG
			printf("Mem Base Address: %#08x%08x ", mmap->base_addr_high, mmap->base_addr_low);
			printf("Length: %x%09x ", mmap->length_high, mmap->length_low);
			printf("type: %d ", mmap->type);
			printf("%s\n", mmap->type == 1 ? "(RAM)":"(RESERVED)" );
			#endif
			if (mmap->type == 1)
			{
				pmmngr_init_region(mmap->base_addr_low, mmap->length_low);
			}

			mmap = (multiboot_memory_map_t*) ((unsigned int) mmap + mmap->size + sizeof(unsigned int));
		}
		printf("\t\t\t\t\t\t\t\t\t"FG_COLOUR_GREEN"[OK]\n");
	}
	else
	{
		printf("\n\t\t\t\t\t\t\t\t\t\t"FG_COLOUR_RED"[FAIL]\n");
	}
}

int kmain(int virt_start, int virt_end, int phy_start, int phy_end, unsigned int ebx)
{
	//char string[32];
	int i = 0;
	uint32_t mem_size = 0;
	uint32_t modules_end = phy_end;
	multiboot_info_t * mbinfo;

	fb_clear();
	gdt_install();
	idt_install();	
	irq_install();

	printf("Setting up int handlers");
	printf("\t\t\t\t\t\t\t\t\t\t" FG_COLOUR_GREEN "[OK]\n");

	mbinfo = (multiboot_info_t *) (ebx + 0xC0000000);
#if 1
	printf("Verifying modules");
	if (mbinfo->flags & MULTIBOOT_FLAGS_MODS)
	{
		unsigned int j;
	#if DEBUG
		printf("Modules struct is valid\n");
		printf("%d modules loaded.\n", mbinfo->mods_count);
	#endif
		multiboot_module_t * mod;
		mod = (multiboot_module_t *) (mbinfo->mods_addr + 0xC0000000);

		for (j = 0; j < mbinfo->mods_count; j++, mod++)
		{
			mod->mod_start += 0xC0000000;
			mod->mod_end += 0xC0000000;
			mod->string += 0xC0000000;
		#if DEBUG
			printf("Module start:\t%#x\n", mod->mod_start);
			printf("Module end:\t\t%#x\n", mod->mod_end);
			printf("Module cmd:\t\t%s\n", (char *)(mod->string));
		#endif
			//printf("Calling module\n");
			if (mod->mod_end > modules_end)
			{
				modules_end = mod->mod_end;
			}
			//call_module_t start_program = (call_module_t) (mod->mod_start);
			//start_program();			
		}
		printf("\t\t\t\t\t\t\t\t\t\t\t" FG_COLOUR_GREEN "[OK]\n");
	}
	else
	{
		printf("\t\t\t\t\t\t\t\t\t\t" FG_COLOUR_RED "[FAIL]\n");
	}	
#else
	ebx = ebx;

#endif	

	printf("Checking memory:\n\t");
	init_memory(modules_end, mem_size, mbinfo);
	pmmngr_deinit_region(phy_start, 0x400000);
	printf("Switch to Kernel managed Virtual Memory");
	vmmngr_initialize();
	printf("\t\t\t\t\t\t"FG_COLOUR_GREEN"[OK]\n");


	printf("Enable interrupt handlers");
	enable_handlers();
	printf("\t\t\t\t\t\t\t\t\t"FG_COLOUR_GREEN"[OK]\n");

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
	printf("CPUID: %x:%x ", cpu_id[2], cpu_id[3]);
	print_vendor_string(cpu_id);
	
	
#if 0
	get_cpuid(1, cpu_id);
	parse_cpuid_features(cpu_id);
#endif
	
	printf("Kernel Physical memory:\t%#08x -> %#08x\n", phy_start, phy_end);
	printf("Kernel Virtual memory:\t%#08x -> %#08x\n", virt_start, virt_end);


	printf("Chris' simple OS. Built: %s", BUILDSTR);

	start_console();

//	while(i)
//	{/
//		printf("%c", getc());
//	}
	return 0;
}