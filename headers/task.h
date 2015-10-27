#ifndef INCLUDE_TASK_H
#define INCLUDE_TASK_H

#include "stdint.h"

typedef struct {
    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} cpu_registers;
 
typedef struct Task {
	int started;
    cpu_registers regs;
    struct Task *next;
} Task;
 
void initTasking();
void createTask(Task*, void(*)(), uint32_t, uint32_t*);

void preempt(); // Switch task frontend
extern void switchTask(cpu_registers *old, cpu_registers *new); // The function which actually switches
extern void custom_switch(cpu_registers *old, cpu_registers *new); // The function which actually switches
extern void launch_task();
#endif