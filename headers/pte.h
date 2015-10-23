#ifndef HEADER_PTE_H
#define HEADER_PTE_H

#include "mmngr_phys.h"
enum PAGE_PTE_FLAGS
{
	I86_PTE_PRESENT			=	0x1,
	I86_PTE_WRITABLE		=	0x2,
	I86_PTE_USER			=	0x4,
	I86_PTE_WRITETHROUGH	=	0x8,
	I86_PTE_NOT_CACHEABLE	=	0x10,
	I86_PTE_ACCESSED		= 	0x20,
	I86_PTE_DIRTY			=	0x40,
	I86_PTE_PAT				=	0x80,
	I86_PTE_CPU_GLOBAL		=	0x100,
	I86_PTE_LV4_GLOBAL		=	0x200,
	I86_PTE_FRAME			=	0x7FFFF000
};

typedef uint32_t pt_entry;

extern inline void pt_entry_add_attrib(pt_entry * e, uint32_t attrib);
extern inline void pt_entry_del_attrib(pt_entry * e, uint32_t attrib);
extern inline void pt_entry_set_frame(pt_entry * e, physical_addr addr);
extern inline uint32_t pt_entry_is_present(pt_entry e);
extern inline uint32_t pt_entry_is_writable(pt_entry e);
extern inline physical_addr pt_entry_pfn(pt_entry e);
#endif