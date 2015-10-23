#ifndef HEADER_MMNGR_VIRTUAL_H
#define HEADER_MMNGR_VIRTUAL_H

#include "pde.h"
#include "pte.h"

typedef uint32_t virtual_addr;

#define PAGES_PER_TABLE 1024
#define PAGES_PER_DIR	1024

#define PAGE_DIRECTORY_INDEX(x) 	(((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x)			(((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)

#define PTABLE_ADDR_SPACE_SIZE 0x400000

#define DTABLE_ADDR_SPACE_SIZE 0x100000000

#define PAGE_SIZE 4096



struct ptable 
{
	pt_entry m_entries[PAGES_PER_TABLE];
};

typedef struct ptable ptable;

struct pdirectory {
	pd_entry m_entries[PAGES_PER_DIR];
};
typedef struct pdirectory pdirectory;


struct pageinfo {
	int pagetable;
	int page;
}__attribute((packed))__;
typedef struct pageinfo pageinfo;

pdirectory * kernel_page_dir = (pdirectory *) 0xFFFFF000;
int vmmngr_alloc_page(pt_entry * e);
void vmmngr_initialize();
void vmmngr_map_page(void * phys, void * virt);
#endif

