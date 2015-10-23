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

#define DEBUG 0
#define BUILDSTR __DATE__  " "  __TIME__  "\n"
typedef void (*call_module_t)(void);
extern char * fb;


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
	fb_set_colour(2, 0);
	printf("\t\t\t\t\t\t\t\t\t\t[OK]\n");
	fb_set_colour(7, 0);

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
		//mod = (multiboot_module_t *) (mbinfo->mods_addr );

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
		fb_set_colour(2, 0);
		printf("\t\t\t\t\t\t\t\t\t\t\t[OK]\n");
		fb_set_colour(7, 0);
	}
	else
	{
		fb_set_colour(4, 0);
		printf("\t\t\t\t\t\t\t\t\t\t[FAIL]\n");
		fb_set_colour(7, 0);
	}	
#else
	ebx = ebx;

#endif	


	printf("Checking memory:\n\t");
	if (mbinfo->flags & MULTIBOOT_FLAGS_MEMINFO)
	{
		//printf("Found %d kB lower memory, %d kB upper memory\n", mbinfo->mem_lower, mbinfo->mem_upper);
		//printf("Found %d kB total memory\n", mbinfo->mem_upper + mbinfo->mem_lower);

		mem_size = mbinfo->mem_upper + mbinfo->mem_lower + 1024;
	}
	else
	{
		printf("\t\t\t\t\t\t\t\t\t\t[FAIL]\n");
		asm("hlt");
	}

	pmmngr_init(mem_size, (uint32_t *) modules_end);
	printf("Initialised memory manager to handle %d kB", mem_size);
	fb_set_colour(2, 0);
	printf("\t\t\t[OK]\n");
	fb_set_colour(7, 0);

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
		fb_set_colour(2, 0);
		printf("\t\t\t\t\t\t\t\t\t[OK]\n");
		fb_set_colour(7, 0);
	}
	else
	{
		printf("\n\t\t\t\t\t\t\t\t\t\t[FAIL]\n");
	}

	pmmngr_deinit_region(phy_start, 0x400000);
	printf("Switch to Kernel managed Virtual Memory");
	vmmngr_initialize();
	fb_set_colour(2, 0);
	printf("\t\t\t\t\t\t[OK]\n");
	fb_set_colour(7, 0);


	printf("Enable interrupt handlers");
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

	fb_set_colour(2, 0);
	printf("\t\t\t\t\t\t\t\t\t[OK]\n");
	fb_set_colour(7, 0);

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
	

	
	printf("Kernel Physical memory from %#x -> %#x\n", phy_start, phy_end);
	printf("Kernel Virtual memory from %#x -> %#x\n", virt_start, virt_end);


	mbinfo = (multiboot_info_t *) (ebx );



	uint32_t * p = (uint32_t *) pmmngr_alloc_block();
	printf("p allocated at %#x\n", p);
	pmmngr_free_block(p);
	p = (uint32_t *) pmmngr_alloc_block();

	for ( i = 0; i < 4096; ++i)
	{
		p = (uint32_t *) pmmngr_alloc_block();
	}
	printf("p allocated at %#x\n", p);

	vmmngr_map_page(p, (void *)0xb0000000);
	int * inp = (int *)0xb0000000;
	*inp = 6;
	printf("Deallocated p to free block 1. p is reallocated to %#x\n", p);
	pmmngr_free_block(p);

	printf("Chris' simple OS. Built: %s", BUILDSTR);
	asm("hlt");

	while(i)
	{
		//i = i;
	}

	return 0;
}