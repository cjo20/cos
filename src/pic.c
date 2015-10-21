#include "asm_func.h"
#include "io.h"
#include "interrupt.h"
#include "fb.h"
#include "lib.h"


#define PIC1			0x20
#define PIC2 			0xA0
#define PIC1_COMMAND	PIC1
#define PIC1_DATA		(PIC1 + 1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA		(PIC2 +  1)

#define PIC1_START_INTERRUPT 	0x0
#define PIC2_START_INTERRUPT 	0x8
#define PIC2_END_INTERRUPT   	PIC2_START_INTERRUPT + 7
#define PIC_ACK 				0x20

#define PIC_READ_IRR	0x0A
#define PIC_READ_ISR	0x0B

/** pic_acknowledge:
* Acknowledges an interrupt from either PIC1 or PIC2
*
* @param num The number of the interrupt
*/
void pic_acknowledge(unsigned int interrupt)
{
	if (interrupt > PIC2_END_INTERRUPT)
	{
		return;
	}

	if (interrupt >= PIC2_START_INTERRUPT)
	{
		outb(PIC2, PIC_ACK);
	}
	
	outb(PIC1, PIC_ACK);
	
}


#define ICW1_ICW4		0x01
#define ICW1_SINGLE		0x02
#define ICW1_INTERVAL4 	0x04
#define ICW1_LEVEL		0x08
#define ICW1_INIT		0x10

#define ICW4_8086		0x01
#define ICW4_AUTO		0x02
#define ICW4_BUF_SLAVE	0x08
#define ICW4_BUF_MASTER	0x0C
#define ICW4_SFNM		0x10

void pic_remap(char offset1, char offset2)
{
	unsigned char a1, a2;

	a1 = inb(PIC1_DATA);
	a2 = inb(PIC2_DATA);

	outb(PIC1_COMMAND, ICW1_INIT + ICW1_ICW4);
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT + ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1);
	io_wait();
	outb(PIC2_DATA, offset2);
	io_wait();
	outb(PIC1_DATA, 0x4);
	io_wait();
	outb(PIC2_DATA, 0x2);
	io_wait();

	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	outb(PIC1_DATA, a1);
	io_wait();
	outb(PIC2_DATA, a2);
	io_wait();
}


char IRQ_get_mask(unsigned char pic)
{
	if (pic == 1)
	{
		return inb(PIC1_DATA);
	}
	else
	{
		return inb(PIC2_DATA);
	}
}

void IRQ_set_mask(unsigned char irq_line)
{
	unsigned short port;
	unsigned char value;

	if (irq_line < 8)
	{
		port = PIC1_DATA;
	}
	else
	{
		port = PIC2_DATA;
		irq_line -= 8;
	}

	value = inb(port) | (1 << irq_line);
	outb(port, value);
}

void IRQ_clear_mask(unsigned char irq_line)
{
	unsigned short port;
	unsigned char value;

	if (irq_line < 8)
	{
		port = PIC1_DATA;
	}
	else
	{
		port = PIC2_DATA;
		irq_line -= 8;
	}

	value = inb(port) & ~(1 << irq_line);
	outb(port, value);
}

void IRQ_disable(char pic)
{
	unsigned short port;
	unsigned char value;

	if (pic == 1)
	{
		port = PIC1_DATA;
	}
	else
	{
		port = PIC2_DATA;
	}

	value = 0xFF;
	outb(port, value);
}

static unsigned short pic_get_irq_reg(int ocw3)
{
	outb(PIC1_COMMAND, ocw3);
	outb(PIC2_COMMAND, ocw3);
	return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

unsigned short pic_get_irr()
{
	return pic_get_irq_reg(PIC_READ_IRR);
}

unsigned short pic_get_isr()
{
	return pic_get_irq_reg(PIC_READ_ISR);
}


extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void * irq_routines[16] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

void irq_install_handler(int irq, void (*handler)(struct cpu_state *r))
{
	irq_routines[irq] = handler;
}


void irq_uninstall_handler(int irq)
{
	irq_routines[irq] = 0;
}


void irq_install()
{
	pic_remap(0x20, 0x28);

	idt_set_gate(0x20, (unsigned)irq0, 0x08, 0x8E);
	idt_set_gate(0x21, (unsigned)irq1, 0x08, 0x8E);
	idt_set_gate(0x22, (unsigned)irq2, 0x08, 0x8E);
	idt_set_gate(0x23, (unsigned)irq3, 0x08, 0x8E);
	idt_set_gate(0x24, (unsigned)irq4, 0x08, 0x8E);
	idt_set_gate(0x25, (unsigned)irq5, 0x08, 0x8E);
	idt_set_gate(0x26, (unsigned)irq6, 0x08, 0x8E);
	idt_set_gate(0x27, (unsigned)irq7, 0x08, 0x8E);

	idt_set_gate(0x28, (unsigned)irq8, 0x08, 0x8E);
	idt_set_gate(0x29, (unsigned)irq9, 0x08, 0x8E);
	idt_set_gate(0x2A, (unsigned)irq10, 0x08, 0x8E);
	idt_set_gate(0x2B, (unsigned)irq11, 0x08, 0x8E);
	idt_set_gate(0x2C, (unsigned)irq12, 0x08, 0x8E);
	idt_set_gate(0x2D, (unsigned)irq13, 0x08, 0x8E);
	idt_set_gate(0x2E, (unsigned)irq14, 0x08, 0x8E);
	idt_set_gate(0x2F, (unsigned)irq15, 0x08, 0x8E);
}

void irq_handler(struct cpu_state *r)
{
	char str[30];
	void (*handler)(struct cpu_state * r);
	handler = irq_routines[r->int_no];

	if (handler)
	{
		//fb_writeString("IRQ handled: ");	
		//utoa(r->int_no, str, 10);
		//fb_writeString(str);
		//fb_writeString("\n");
		handler(r);
	}
	else
	{
		
		fb_writeString("IRQ not handled: ");	
		utoa(r->int_no, str, 10);
		fb_writeString(str);
		fb_writeString("\n");
	}
	
	pic_acknowledge(r->int_no);
}

void irq_spurious_handler(struct cpu_state * r)
{
	unsigned short isr = pic_get_isr();

	if (!(isr & (1 << r->int_no)))
	{
		//fb_writeString("SPR IRQ");
		//char int_no[7];
		//utoa(r->int_no, int_no, 10);
		//fb_writeString(int_no);
		//fb_writeString("\n");
		if (r->int_no == 15)
		{
			outb(PIC1, PIC_ACK);
		}
	}
}