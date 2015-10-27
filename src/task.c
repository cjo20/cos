#include "task.h"
#include "mmngr_phys.h"
#include "mmngr_virtual.h"
#include "fb.h"
#include "pic.h"

static Task * runningTask;
static Task kernel;
static Task otherTask;


void dumpTask(Task * t)
{
	printf("Regs:\nEAX %08x\tEBX %08x\tECX %#08x\nEDX %08x\tESI %08x\tEDI %#08x\n",
				t->regs.eax, t->regs.ebx, t->regs.ecx, t->regs.edx, t->regs.esi, t->regs.edi);
	printf("FLG %#08x\tEIP %#08x\tCR3 %#08x\nESP %#08x", t->regs.eflags, t->regs.eip, t->regs.cr3, t->regs.esp);
}

static void otherMain()
{
	static int i = 0;
	while(1)
	{
		fb_write_cell(78, i + '0', 7, 0);
		i++;
		i %= 10;
		//asm("pushf; pop %%eax; mov %%eax, %0;" : "=d"(i) : : "%eax");
		//printf("othermain EFLAGS: %#x\n", i);
		//asm("xchg %bx, %bx");
		//asm("int $0x20");
		//printf("Hello multitasking world!\n");
		//preempt();
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
	task->started = 0;
	//int i = 0;
	task->regs.eax = 0;
	task->regs.ebx = 0;
	task->regs.ecx = 0;
	task->regs.edx = 0;
	task->regs.esi = 0;
	task->regs.edi = 0;
	task->regs.eflags = flags | (1 << 9);
	task->regs.eip = (uint32_t) main;
	task->regs.cr3 = (uint32_t) pagedir;

	vmmngr_map_page( pmmngr_alloc_block() , (void *) 0xC0000000 - 0x10);
	task->regs.esp = (uint32_t) 0xC0000000 - 0x10;

	*(uint32_t *)(task->regs.esp) = task->regs.eflags;
	task->regs.esp -= 4;
	*(uint32_t *)(task->regs.esp) = 0x8;
	task->regs.esp -= 4;
	*(uint32_t *)(task->regs.esp) = (uint32_t) main;

	/*i = 1;
	*((uint32_t *)(task->regs.esp - 4 * i)) = 0x0;
	i++;
	*((uint32_t *)(task->regs.esp - 4 * i)) = 0x0;
	i++;
	*((uint32_t *)(task->regs.esp - 4 * i)) = flags;
	i++;
	*((uint32_t *)(task->regs.esp - 4 * i)) = 0x8;
	i++;
	*((uint32_t *)(task->regs.esp - 4 * i)) = (uint32_t)main;
	i++;

	for (; i <= 15; ++i)
	{
		*((uint32_t *)(task->regs.esp - 4 * i)) = 0x0;
	}

	for (; i <= 19; ++i)
	{
		*((uint32_t*)(task->regs.esp - 4 * i)) = 0x10;
	}

	i--;
	// *((uint32_t*)(task->regs.esp - 4 * i)) = 0;
	task->regs.esp -= i * 4;*/

	task->next = 0;
}

void preempt()
{
	Task * last = runningTask;
	runningTask = runningTask->next;

	if (!runningTask->started)
	{
		pic_acknowledge(0);
		runningTask->started = 1;
	}

#if 0
	printf("From %#08x\n", last);
	dumpTask(last);
	printf("\nTo %#08x\n", runningTask);
	dumpTask(runningTask);
	printf("\n\n");
#endif
//	asm("xchg %bx, %bx");
	custom_switch(&last->regs, &runningTask->regs);
}