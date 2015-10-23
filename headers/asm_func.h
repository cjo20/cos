#ifndef INCLUDE_ASM_H
#define INCLUDE_ASM_H

#include "stdint.h"
void flush_gdt(void * gdt_ptr);
void io_wait();
void get_cpuid(int info, int cpu_id[4]);
void get_low_memory(int * lowmem);

void pmmngr_load_PDBR (uint32_t addr);
uint32_t pmmngr_get_PDBR();
void flush_TLB(uint32_t addr);
void pmmngr_paging_enable(int enable);
#endif
