#ifndef INCLUDE_INTERRUPT_HANDLER_H
#define INCLUDE_INTERRUPT_HANDLER_H
#include "interrupt.h"
#include "mem.h"
#include "fb.h"
#include "io.h"
#include "pic.h"

void interrupt_handler(struct cpu_state cpu, struct stack_state stack, unsigned int interrupt)
{
	asm ("xchg %bx,%bx");
	switch (interrupt)
	{
		case 0:
			fb_write("i\n", 2);
			break;

		case 0x20:
			pic_acknowledge(interrupt);
			break;

		case 0x21:
			fb_write("k\n", 2);
			pic_acknowledge(interrupt);
			break;
		default:
			break;
	}
	
	cpu = cpu;
	stack = stack;

	return;
}

struct idt_entry idt[256];
struct idt_ptr idtp;

//char idt_make_flags(char type, char segment, char priv, char present)
//{
//	char flags = 0;
//
//	flags |= (type & 0x7);
//	flags |= (segment & 0x1) << 3;
//	flags |= (priv & 0x3) << 4;
//	flags |= (present & 0x1) << 6;
//	return flags;
//}
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
{
	idt[num].base_lo = base & 0xFFFF;
	idt[num].base_hi = (base >> 16) & 0xFFFF;
	idt[num].flags = flags;
	idt[num].sel = sel;
	idt[num].always0 = 0;
}

void idt_install()
{
	idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
	idtp.base = idt;

	memset(&idt, 0, sizeof(struct idt_entry) * 256);

	idt_set_gate(0, (unsigned long) &interrupt_handler_0, 0x8, 0x8E);
	idt_set_gate(0x20, (unsigned long) &interrupt_handler_32, 0x8, 0x8E);
	idt_set_gate(0x21, (unsigned long) &interrupt_handler_33, 0x8, 0x8E);
	load_idt(&idtp);
}






#endif