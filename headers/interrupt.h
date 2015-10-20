#ifndef INCLUDE_INTERRUPT_H
#define INCLUDE_INTERRUPT_H


struct idt_entry
{
	unsigned short base_lo;
	unsigned short sel;
	unsigned char always0;
	unsigned char flags;
	unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr
{
	unsigned short limit;
	struct idt_entry * base;
} __attribute__((packed));

struct cpu_state 
{
	unsigned int gs, fs, es, ds;
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
	unsigned int int_no, err_code;
	unsigned int eip, cs, eflags, useresp, ss;
} __attribute__((packed));


void interrupt_handler(struct cpu_state * cpu);
void load_idt(void * idt_ptr);
void idt_install();

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);

#endif