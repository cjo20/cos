#include "task.h"
#include "mmngr_phys.h"
#include "mmngr_virtual.h"
#include "fb.h"

static Task * runningTask;
static Task kernel;
static Task otherTask;

static void otherMain()
{
	static int i = 0;
	while(1)
	{
		fb_write_cell(78, i + '0', 7, 0);
		i++;
		i %= 10;
		//printf("Hello multitasking world!\n");
		preempt();
	}
}


void initTasking()
{

	asm volatile("movl %%cr3, %%eax; movl %%eax, %0;" : "=m" (kernel.regs.cr3)::"%eax");
	asm volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;": "=m" (kernel.regs.eflags)::"%eax");

	createTask(&otherTask, otherMain, kernel.regs.eflags, (uint32_t*)kernel.regs.cr3);

	kernel.next = &otherTask;
	otherTask.next = &kernel;

	runningTask = &kernel;
}

void createTask(Task * task, void (*main)(), uint32_t flags, uint32_t * pagedir)
{
	task->regs.eax = 0;
	task->regs.ebx = 0;
	task->regs.ecx = 0;
	task->regs.edx = 0;
	task->regs.esi = 0;
	task->regs.edi = 0;
	task->regs.eflags = flags;
	task->regs.eip = (uint32_t) main;
	task->regs.cr3 = (uint32_t) pagedir;

	vmmngr_map_page( pmmngr_alloc_block() , (void *) 0xC0000000 - 0x10);
	task->regs.esp = (uint32_t) 0xC0000000 - 0x10;
	task->next = 0;
}

void preempt()
{
	
	Task * last = runningTask;
	runningTask = runningTask->next;
	switchTask(&last->regs, &runningTask->regs);
}