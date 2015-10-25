#include "mmngr_virtual.h"
#include "stdint.h"
#include "asm_func.h"
#include "mem.h"
#include "fb.h"
pdirectory * _cur_directory = 0;
pdirectory * kernel_page_dir = (pdirectory *) 0xFFFFF000;

pageinfo vmmngr_virt_to_page_index(void * addr)
{
	pageinfo pginf;
	virtual_addr vaddr = (virtual_addr) addr;
	vaddr &= ~0xFFF;
	pginf.pagetable = vaddr >> 22;
	pginf.page = (vaddr <<10) >> 22;
	return pginf;
}

ptable * vmmngr_get_ptable_address(virtual_addr addr)
{
	return (ptable *) (PAGE_TABLES_ADDRESS + PAGE_DIRECTORY_INDEX(addr) * PAGE_SIZE);
}

inline pt_entry * vmmngr_ptable_lookup_entry(ptable * p, virtual_addr addr)
{
	if (p)
	{
		return &p->m_entries[ PAGE_TABLE_INDEX(addr) ];
	}

	return 0;
}

inline pd_entry * vmmngr_pdirectory_lookup_entry(pdirectory * p, virtual_addr addr)
{
	if (p)
	{
		return &p->m_entries[ PAGE_DIRECTORY_INDEX(addr)];
	}

	return 0;
}

int vmmngr_commit_page(pt_entry * e)
{
	void *p = pmmngr_alloc_block();

	if (!p)
	{
		return 0;
	}

	pt_entry_set_frame(e, (physical_addr) p);
	pt_entry_add_attrib(e, I86_PTE_PRESENT);
	pt_entry_add_attrib(e, I86_PTE_WRITABLE);

	return 1;
}

int vmmngr_alloc_page(virtual_addr virt)
{
	pdirectory * pd = (pdirectory *) PAGE_DIRECTORY_ADDRESS;
	pd_entry * pde = &pd->m_entries[PAGE_DIRECTORY_INDEX(virt)];

	if ((!pd) || (!pde))
	{
		return 0;
	}

	if (!pd_entry_is_present(*pde))
	{
		ptable *newpt = pmmngr_alloc_block();

		if (!newpt)
		{
			return 0;
		}

		*pde = (pd_entry) 0;

		pd_entry_add_attrib(pde, I86_PDE_PRESENT);
		pd_entry_add_attrib(pde, I86_PDE_WRITABLE);
		pd_entry_set_frame(pde, (physical_addr) newpt);

		memset(vmmngr_get_ptable_address(virt), 0, sizeof(ptable));

	}

	ptable * pt = vmmngr_get_ptable_address(virt);

	pt_entry *pte = &pt->m_entries[PAGE_TABLE_INDEX(virt)];

	if  (pt_entry_is_present(*pte))
	{
		return 1;
	}

	if (!vmmngr_commit_page(pte))
	{
		return 0;
	}

	return 1;	
}

void vmmngr_set_user(virtual_addr addr, int user)
{
	ptable * ptable = vmmngr_get_ptable_address(addr);
	pt_entry * pte = vmmngr_ptable_lookup_entry(ptable, addr);
	if (user)
	{
		pt_entry_add_attrib(pte, I86_PTE_USER);
	}
	else
	{
		pt_entry_del_attrib(pte, I86_PTE_USER);
	}
}

void vmmngr_set_writeable(virtual_addr addr, int writeable)
{
	ptable * ptable = vmmngr_get_ptable_address(addr);
	pt_entry * pte = vmmngr_ptable_lookup_entry(ptable, addr);
	if (writeable)
	{
		pt_entry_add_attrib(pte, I86_PTE_WRITABLE);
	}
	else
	{
		pt_entry_del_attrib(pte, I86_PTE_WRITABLE);
	}

}

void vmmngr_free_page(virtual_addr addr)
{
	pt_entry * pte;

	pte = (pt_entry *) ((uint32_t) vmmngr_get_ptable_address(addr) + PAGE_TABLE_INDEX(addr));

	pmmngr_free_block((void *) pt_entry_pfn(*pte));

	pt_entry_del_attrib(pte, I86_PTE_PRESENT);
}

/*
int vmmngr_alloc_page(pt_entry * e)
{
	void * p = pmmngr_alloc_block();
	if (!p)
	{
		return 0;
	}

	pt_entry_set_frame(e, (physical_addr) p);
	pt_entry_add_attrib(e, I86_PTE_PRESENT);

	return 1;
}*/

/*void vmmngr_free_page(pt_entry * e)
{
	void * p = (void *) pt_entry_pfn(*e);

	if (p)
	{
		pmmngr_free_block(p);
	}

	pt_entry_del_attrib(e, I86_PTE_PRESENT);
}*/


inline int vmmngr_switch_pdirectory(pdirectory * dir)
{
	if (!dir)
	{
		return 0;
	}

	_cur_directory = dir;
	pmmngr_load_PDBR((physical_addr) _cur_directory->m_entries);
	return 1;
}

inline pdirectory * vmmngr_get_directory()
{
	return _cur_directory;
}

void vmmngr_flush_tld_entry(virtual_addr addr)
{
	flush_TLB(addr);
}

void vmmngr_map_page(void * phys, void * virt)
{
	pageinfo pginf = vmmngr_virt_to_page_index(virt);
	pdirectory * page_directory = kernel_page_dir; //vmmngr_get_directory();

	pd_entry * e = &page_directory->m_entries[PAGE_DIRECTORY_INDEX((uint32_t) virt)];
	if (!pd_entry_is_present(*e))
	{
		ptable * new_table = (ptable *) pmmngr_alloc_block();
		if (!new_table)
		{
			return;
		}

		ptable * virtual_table = (ptable *) (0xFFC00000 + (pginf.pagetable * 0x1000));
		pd_entry * entry = &kernel_page_dir->m_entries[pginf.pagetable];

		pd_entry_add_attrib(entry, I86_PDE_PRESENT);
		pd_entry_add_attrib(entry, I86_PDE_WRITABLE);
		pd_entry_set_frame(entry, (physical_addr) new_table);

		memset(virtual_table, 0, sizeof(ptable));
		pt_entry * page = &virtual_table->m_entries[pginf.page];
		pt_entry_set_frame(page, (physical_addr) phys);
		pt_entry_add_attrib(page, I86_PTE_PRESENT);
		pt_entry_add_attrib(page, I86_PTE_WRITABLE);
		return;

		
	}

	ptable * table = (ptable *) PAGE_GET_PHYSICAL_ADDRESS(e);

	pt_entry * page = &table->m_entries[PAGE_TABLE_INDEX((uint32_t) virt)];

	pt_entry_set_frame(page, (physical_addr) phys);
	pt_entry_add_attrib(page, I86_PTE_PRESENT);
	pt_entry_add_attrib(page, I86_PTE_WRITABLE);
}

void vmmngr_initialize(physical_addr dir_address)
{
	int i = 0;
	int frame = 0;
	int virt = 0;

	ptable * table = (ptable *) pmmngr_alloc_block();
	if (!table)
	{
		return;
	}

	ptable * table2 = (ptable *) pmmngr_alloc_block();
	if (!table2)
	{
		pmmngr_free_block(table);
		return;
	}	

	memset(table, 0, sizeof(ptable));

	for (i = 0, frame = 0x0, virt = 0x00000000; i < 1024; ++i, frame+=4096, virt+=4096)
	{
		pt_entry page = 0;
		pt_entry_add_attrib(&page, I86_PTE_PRESENT);
		pt_entry_set_frame(&page, frame);

		table->m_entries[PAGE_TABLE_INDEX(virt)] = page;
	}

	for (i = 0, frame = 0x0, virt = 0xc0000000; i < 1024; ++i, frame+=4096, virt+=4096)
	{
		pt_entry page = 0;
		pt_entry_add_attrib(&page, I86_PTE_PRESENT);
		pt_entry_set_frame(&page, frame);

		table2->m_entries[PAGE_TABLE_INDEX(virt)] = page;
	}

	pdirectory * dir = (pdirectory *) dir_address;// pmmngr_alloc_block();

	if (!dir)
	{
		pmmngr_free_block(table);
		pmmngr_free_block(table2);
		return;
	}

	memset(dir, 0, sizeof(pdirectory));

	pd_entry * entry = &dir->m_entries[PAGE_DIRECTORY_INDEX(0x00000000)];
	pd_entry_add_attrib(entry, I86_PDE_PRESENT);
	pd_entry_add_attrib(entry, I86_PDE_WRITABLE);
	pd_entry_set_frame(entry, (physical_addr) table);

	pd_entry * entry2 = &dir->m_entries[PAGE_DIRECTORY_INDEX(0xc0000000)];
	pd_entry_add_attrib(entry2, I86_PDE_PRESENT);
	pd_entry_add_attrib(entry2, I86_PDE_WRITABLE);
	pd_entry_set_frame(entry2, (physical_addr) table2);

	pd_entry * entry3 = &dir->m_entries[PAGE_DIRECTORY_INDEX(0xFFC00000)];
	pd_entry_add_attrib(entry3, I86_PDE_PRESENT);
	pd_entry_add_attrib(entry3, I86_PDE_WRITABLE);
	pd_entry_set_frame(entry3, (physical_addr) dir);

	vmmngr_switch_pdirectory(dir);
	pmmngr_paging_enable(1);

}

//pdirectory * clone_directory(pdirectory * src)
//{
//	uint32_t phys;
//}