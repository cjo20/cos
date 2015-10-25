#include "gdt.h"
#include "asm_func.h"

struct gdt_ptr gp;

#define GDT_ADDRESS 0xFFBB9000

void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
	gp.base[num].base_low = (base & 0xFFFF);
	gp.base[num].base_middle = (base >> 16) & 0xFF;
	gp.base[num].base_high = (base >> 24) & 0xFF;

	gp.base[num].limit_low = (limit & 0xFFFF);
	gp.base[num].granularity = (limit >> 16) & 0x0F;
	gp.base[num].granularity |= (gran & 0xF0);
	gp.base[num].access = access;
}

void gdt_install()
{
	
	gp.limit = (sizeof(struct gdt_entry) * 5) - 1;
	gp.base = (struct gdt_entry * ) GDT_ADDRESS;
	
	gdt_set_gate(0, 0, 0 ,0 ,0);
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0XCF); // Data segment
	gdt_set_gate(3, 0x0, 0xFFFFFFFF, 0xFA, 0xCF); //User mode code segment
	gdt_set_gate(4, 0x0, 0xFFFFFFFF, 0xF2, 0xCF); //User mode data segment	
	flush_gdt(&gp);
}
