#ifndef INCLUDE_PIC_H
#define INCLUDE_PIC_H
void pic_remap(char offset1, char offset2);
void IRQ_set_mask(unsigned char irq_line);
void IRQ_clear_mask(unsigned char irq_line);
void pic_acknowledge(unsigned int interrupt);
char IRQ_get_mask(unsigned char pic);
void IRQ_disable(char pic);

unsigned short pic_get_irr();
unsigned short pic_get_isr();
#endif