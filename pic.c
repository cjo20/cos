#include "asm_func.h"
#include "io.h"

#define PIC1			0x20
#define PIC2 			0xA0
#define PIC1_COMMAND	PIC1
#define PIC1_DATA		(PIC1 + 1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA		(PIC2 +  1)

#define PIC1_START_INTERRUPT 	0x20
#define PIC2_START_INTERRUPT 	0x28
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
	if (interrupt < PIC1_START_INTERRUPT || interrupt > PIC2_END_INTERRUPT)
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
	outb(PIC2_COMMAND, ICW1_INIT + ICW1_ICW4);
	outb(PIC1_DATA, offset1);
	outb(PIC2_DATA, offset2);
	outb(PIC1_DATA, 0x4);
	outb(PIC2_DATA, 0x2);

	outb(PIC1_DATA, ICW4_8086);
	outb(PIC2_DATA, ICW4_8086);

	outb(PIC1_DATA, a1);
	outb(PIC2_DATA, a2);
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