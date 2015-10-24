#include "gdt.h"
#include "asm_func.h"
struct gdt_entry gdt[5];
struct gdt_ptr gp;



void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
	gdt[num].base_low = (base & 0xFFFF);
	gdt[num].base_middle = (base >> 16) & 0xFF;
	gdt[num].base_high = (base >> 24) & 0xFF;

	gdt[num].limit_low = (limit & 0xFFFF);
	gdt[num].granularity = (limit >> 16) & 0x0F;
	gdt[num].granularity |= (gran & 0xF0);
	gdt[num].access = access;
}

void gdt_install()
{
	gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
	gp.base = gdt;
	
	gdt_set_gate(0, 0, 0 ,0 ,0);
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0XCF); // Data segment
	gdt_set_gate(3, 0x0, 0xFFFFFFFF, 0xFA, 0xCF); //User mode code segment
	gdt_set_gate(4, 0x0, 0xFFFFFFFF, 0xF2, 0xCF); //User mode data segment	
	flush_gdt(&gp);
}
