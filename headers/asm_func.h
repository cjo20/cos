#ifndef INCLUDE_ASM_H
#define INCLUDE_ASM_H

void flush_gdt(void * gdt_ptr);
void io_wait();
void get_cpuid(int info, int cpu_id[4]);
#endif
