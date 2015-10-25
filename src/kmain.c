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
#include "task.h"
#include "kmalloc.h"

#define DEBUG 0
#define BUILDSTR __DATE__  " "  __TIME__  "\n"
typedef void (*call_module_t)(void);
extern char * fb;

#define MEMBITMAP_PHYSICAL_ADDRESS 	0x00200000
#define MEMBITMAP_VIRTUAL_ADDRESS	0xFFBBA000
#define VIDMEM_PHYSICAL_ADDRESS		0x000B8000
#define VIDMEM_VIRTUAL_ADDRESS		0xFFBFA000
#define PAGEDIR_PHYSICAL_ADDRESS	0x00080000
#define PAGEDIR_VIRTUAL_ADDRESS		0xFFFFF000
#define GDT_PHYSICAL_ADDRESS		0x00000000
#define GDT_VIRTUAL_ADDRESS			0xFFBB9000

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

void init_memory(unsigned int location, multiboot_info_t * mbinfo)
{
	uint32_t mem_size = 0; 
	uint32_t i = 0;

	location = location;


	if (mbinfo->flags & MULTIBOOT_FLAGS_MEMINFO)
	{
		printf("Total memory: %dKiB\n", mbinfo->mem_upper + mbinfo->mem_lower + 1024);
	}

	if (mbinfo->flags & MULTIBOOT_FLAGS_MMAP)
	{

		multiboot_memory_map_t * mmap = 0;
		multiboot_memory_map_t * mmap_end = 0;
		
		mbinfo->mmap_addr += 0xC0000000;
		mmap_end = (multiboot_memory_map_t *) (mbinfo->mmap_addr + mbinfo->mmap_length);
		

		printf("Parsing GRUB memory map...");
		mmap = (multiboot_memory_map_t *) mbinfo->mmap_addr;
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
				mem_size += mmap->length_low;
				//pmmngr_init_region(mmap->base_addr_low, mmap->length_low);
			}

			mmap = (multiboot_memory_map_t*) ((unsigned int) mmap + mmap->size + sizeof(unsigned int));
		}
		printf("done\n");
	
		pmmngr_init(mem_size, (uint32_t *) MEMBITMAP_PHYSICAL_ADDRESS);
		printf("Initialised physical memory manager to handle %d KiB\n", mem_size >> 10);	

		printf("Parsing GRUB regions...");
		mmap = (multiboot_memory_map_t *) mbinfo->mmap_addr;
		while (mmap < mmap_end)
		{
			if (mmap->type == 1)
			{
				pmmngr_init_region(mmap->base_addr_low, mmap->length_low);
			}

			mmap = (multiboot_memory_map_t*) ((unsigned int) mmap + mmap->size + sizeof(unsigned int));
		}
		printf("done\n");

		uint32_t usable_memory = pmmngr_get_free_block_count() * PAGE_SIZE >> 10;
		printf("Initialised %d KiB usable memory\n", usable_memory);

		pmmngr_deinit_region(0x00, 0x1000); // GDT
		pmmngr_deinit_region(0x7E000, 0x2000); //Kernel stack (8192k)
		pmmngr_deinit_region(0xA0000, 0x60000); // VRAM/ROM
		pmmngr_deinit_region(MEMBITMAP_PHYSICAL_ADDRESS, 0x40000); //Physical memory manager bitmap
		pmmngr_deinit_region(PAGEDIR_PHYSICAL_ADDRESS, 0x4000); //Physical memory manager bitmap
		pmmngr_deinit_region(0xC0000000, 0x100000); // 1MB max

		printf("Switch to kernel managed virtual memory...");
		vmmngr_initialize((physical_addr) PAGEDIR_PHYSICAL_ADDRESS);
		printf("done\n");

		vmmngr_map_page((void *)VIDMEM_PHYSICAL_ADDRESS, (void *)VIDMEM_VIRTUAL_ADDRESS);
		vmmngr_map_page((void *)VIDMEM_PHYSICAL_ADDRESS + 4096, (void *)VIDMEM_VIRTUAL_ADDRESS+ 4096);
		vmmngr_map_page((void *)VIDMEM_PHYSICAL_ADDRESS + 8192, (void *)VIDMEM_VIRTUAL_ADDRESS + 8192);
		vmmngr_map_page((void *)VIDMEM_PHYSICAL_ADDRESS + 12228, (void *)VIDMEM_VIRTUAL_ADDRESS + 12228);

		fb_set_vid_mem((char *) VIDMEM_VIRTUAL_ADDRESS);

		for (i = 0; i < 64; ++i)
		{
			vmmngr_map_page((void *) (MEMBITMAP_PHYSICAL_ADDRESS + i * 4096),
							(void *) (MEMBITMAP_VIRTUAL_ADDRESS + i * 4096));
		}

		printf("Switch to virtual bitmap address...");
		pmmngr_set_bitmap_address((uint32_t *) MEMBITMAP_VIRTUAL_ADDRESS);
		printf("done\n");

		printf("Installing GDT...");
		vmmngr_map_page((void *) GDT_PHYSICAL_ADDRESS, (void *)GDT_VIRTUAL_ADDRESS);
		gdt_install();
		printf("done\n");

		//Don't identity-map first 4MB any longer
		pd_entry * pde = vmmngr_pdirectory_lookup_entry((pdirectory *) PAGEDIR_VIRTUAL_ADDRESS, 0x00000000);
		pd_entry_del_attrib(pde, I86_PDE_PRESENT);

	
	}
	else
	{
		printf(FG_COLOUR_RED"Failed to read GRUB memory information\n");
	}
}

int kmain(int virt_start, int virt_end, int phy_start, int phy_end, unsigned int ebx)
{
	//char string[32];
	int i = 0;
	uint32_t modules_end = phy_end;
	multiboot_info_t * mbinfo;

	fb_clear();


	printf("Kernel size: %#x\n", phy_end - phy_start);

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
	init_memory(modules_end, mbinfo);

	idt_install();	
	irq_install();	



	printf("Init Kernel heap");
	if(kmalloc_init(virt_end))
	{
		printf("\t\t\t\t\t\t\t\t\t\t\t"FG_COLOUR_GREEN"[OK]\n");
	}
	else
	{
		printf("\t\t\t\t\t\t\t\t\t\t\t" FG_COLOUR_RED "[FAIL]\n");
	}

	printf("Enable hardware interrupt handlers");
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

	initTasking();
    printf("Switching to otherTask... \n");
    preempt();
    printf("Returned to mainTask!\n");

	start_console();


//	while(i)
//	{/
//		printf("%c", getc());
//	}
	return 0;
}