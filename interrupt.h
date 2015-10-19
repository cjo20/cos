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
	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;
	unsigned int esp;
	unsigned int ebp;
	unsigned int esi;
	unsigned int edi;
} __attribute__((packed));

struct stack_state 
{
	unsigned int error_code;
	unsigned int eip;
	unsigned int cs;
	unsigned int eflags;
} __attribute__((packed));

void interrupt_handler(struct cpu_state cpu, struct stack_state stack, unsigned int interrupt);
void load_idt(void * idt_ptr);
void idt_install();

void interrupt_handler_0();
void interrupt_handler_32();
void interrupt_handler_33();
#endif