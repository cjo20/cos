#ifndef INCLUDE_PIC_H
#define INCLUDE_PIC_H

#include "interrupt.h"

void pic_acknowledge(unsigned int interrupt);

void IRQ_set_mask(unsigned char irq_line);
void IRQ_clear_mask(unsigned char irq_line);
char IRQ_get_mask(unsigned char pic);
void IRQ_disable(char pic);

void irq_install();
void irq_install_handler(int irq, void (*handler)(struct cpu_state *r));
void irq_uninstall_handler(int irq);

void irq_spurious_handler(struct cpu_state * r);

unsigned short pic_get_irr();
unsigned short pic_get_isr();
#endif