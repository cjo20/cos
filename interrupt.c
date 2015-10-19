#ifndef INCLUDE_INTERRUPT_HANDLER_H
#define INCLUDE_INTERRUPT_HANDLER_H
#include "interrupt.h"
#include "mem.h"
#include "fb.h"
#include "io.h"

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

char * exception_messages[] = 
{
	"Division by Zero",
	"Debug",
	"Non Maskable Interrupt",
	"Breakpoint",
	"Overflow",
	"Bounds",
	"Invalid Opcode",
	"Coprocessor not available",
	"Double fault",
	"Coprocessor Segment Overrun",
	"Invalid Task State Segment",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"0x0F Reserved",
	"Math Fault",
	"Alignment Check",
	"Machine Check",
	"SIMD FP Exception",
	"0x14 Reserved",
	"0x15 Reserved",
	"0x16 Reserved",
	"0x17 Reserved",
	"0x18 Reserved",
	"0x19 Reserved",
	"0x1A Reserved",
	"0x1B Reserved",
	"0x1C Reserved",
	"0x1D Reserved",
	"0x1E Reserved",
	"0x1F Reserved",
};


void interrupt_handler(struct cpu_state * cpu)
{

	if (cpu->int_no < 32)
	{
		char * str = ". Exception. System Halted!\n";
		fb_write(exception_messages[cpu->int_no], strlen(exception_messages[cpu->int_no]));
		fb_write(str, strlen(str));

		for(;;);
	}

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

	idt_set_gate(0, (unsigned long) (unsigned)isr0, 0x8, 0x8E);
	idt_set_gate(1, (unsigned long) (unsigned)isr1, 0x8, 0x8E);
	idt_set_gate(2, (unsigned long) (unsigned)isr2, 0x8, 0x8E);
	idt_set_gate(3, (unsigned long) (unsigned)isr3, 0x8, 0x8E);
	idt_set_gate(4, (unsigned long) (unsigned)isr4, 0x8, 0x8E);
	idt_set_gate(5, (unsigned long) (unsigned)isr5, 0x8, 0x8E);
	idt_set_gate(6, (unsigned long) (unsigned)isr6, 0x8, 0x8E);
	idt_set_gate(7, (unsigned long) (unsigned)isr7, 0x8, 0x8E);
	idt_set_gate(8, (unsigned long) (unsigned)isr8, 0x8, 0x8E);
	idt_set_gate(9, (unsigned long) (unsigned)isr9, 0x8, 0x8E);
	idt_set_gate(10, (unsigned long) (unsigned)isr10, 0x8, 0x8E);
	idt_set_gate(11, (unsigned long) (unsigned)isr11, 0x8, 0x8E);
	idt_set_gate(12, (unsigned long) (unsigned)isr12, 0x8, 0x8E);
	idt_set_gate(13, (unsigned long) (unsigned)isr13, 0x8, 0x8E);
	idt_set_gate(14, (unsigned long) (unsigned)isr14, 0x8, 0x8E);
	idt_set_gate(15, (unsigned long) (unsigned)isr15, 0x8, 0x8E);
	idt_set_gate(16, (unsigned long) (unsigned)isr16, 0x8, 0x8E);
	idt_set_gate(17, (unsigned long) (unsigned)isr17, 0x8, 0x8E);
	idt_set_gate(18, (unsigned long) (unsigned)isr18, 0x8, 0x8E);
	idt_set_gate(19, (unsigned long) (unsigned)isr19, 0x8, 0x8E);
	idt_set_gate(20, (unsigned long) (unsigned)isr20, 0x8, 0x8E);
	idt_set_gate(21, (unsigned long) (unsigned)isr21, 0x8, 0x8E);
	idt_set_gate(22, (unsigned long) (unsigned)isr22, 0x8, 0x8E);
	idt_set_gate(23, (unsigned long) (unsigned)isr23, 0x8, 0x8E);
	idt_set_gate(24, (unsigned long) (unsigned)isr24, 0x8, 0x8E);
	idt_set_gate(25, (unsigned long) (unsigned)isr25, 0x8, 0x8E);
	idt_set_gate(26, (unsigned long) (unsigned)isr26, 0x8, 0x8E);
	idt_set_gate(27, (unsigned long) (unsigned)isr27, 0x8, 0x8E);
	idt_set_gate(28, (unsigned long) (unsigned)isr28, 0x8, 0x8E);
	idt_set_gate(29, (unsigned long) (unsigned)isr29, 0x8, 0x8E);
	idt_set_gate(30, (unsigned long) (unsigned)isr30, 0x8, 0x8E);
	idt_set_gate(31, (unsigned long) (unsigned)isr31, 0x8, 0x8E);	
	load_idt(&idtp);
}

#endif