#include "kmalloc.h"
#include "mmngr_virtual.h"
#include "fb.h"

heap_t * kheap = 0;
uint32_t placement_address;

static int32_t find_smallest_hole(uint32_t size, uint8_t page_align, heap_t * heap)
{
	uint32_t iterator = 0;

	while (iterator < heap->index.size)
	{
		header_t * header = (header_t *) lookup_ordered_array(iterator, &heap->index);

		if (page_align > 0)
		{
			uint32_t location = (uint32_t) header;

			int32_t offset = 0;

			if (((location + sizeof(header_t)) & 0xFFFFF000) != 0)
			{
				offset = 0x1000 /* page_size */ - (location + sizeof(header_t)) % 0x1000;
			}

			int32_t hole_size = (int32_t) header->size - offset;

			if (hole_size >= (int32_t) size)
			{
				break;
			}
		}
		else if (header->size >= size)
		{
			break;
		}

		iterator++;
	}

	if (iterator == heap->index.size)
	{
		return -1;
	}
	else
	{

		return iterator;
	}
}

static int8_t header_t_less_than(void * a, void * b)
{
	return (((header_t *) a)->size < ((header_t *)b)->size) ? 1 : 0;
}

heap_t * create_heap(uint32_t start, uint32_t end_addr, uint32_t max, uint8_t supervisor, uint8_t readonly)
{
	
	heap_t * heap = (heap_t*) kmalloc(sizeof(heap_t));

	if (start % 0x1000 || end_addr % 0x1000)
	{
		return 0;
	}

	heap->index = place_ordered_array((void *) start, KHEAP_INDEX_SIZE, &header_t_less_than);

	start += sizeof(type_t) * KHEAP_INDEX_SIZE;

	if ((start & 0xFFFFF000) != 0)
	{
		start &= 0xFFFFF000;
		start += 0x1000;
	}

	heap->start_address = start;
	heap->end_address = end_addr;
	heap->max_address = max;
	heap->supervisor = supervisor;
	heap->readonly = readonly;

	header_t * hole = (header_t *) start;
	hole->size = end_addr - start;
	hole->magic = KHEAP_MAGIC;
	hole->is_hole = 1;
	insert_ordered_array((void *) hole, &heap->index);

	return heap;
}

static void expand(uint32_t new_size, heap_t * heap)
{
	if (new_size <= heap->end_address - heap->start_address)
	{
		return;
	}

	if ((new_size & 0xFFFFF000) != 0)
	{
		new_size &= 0xFFFFF000;
		new_size += 0x1000;
	}

	if (heap->start_address + new_size > heap->max_address)
	{
		return;
	}

	uint32_t old_size = heap->end_address - heap->start_address;
	uint32_t i = old_size;

	while (i < new_size)
	{
		vmmngr_alloc_page(heap->start_address + i);
		vmmngr_set_user(heap->start_address + i, (heap->supervisor)? 0 : 1 );
		vmmngr_set_writeable(heap->start_address + i, (heap->readonly)? 0 : 1 );
		i += 0x1000;
	}

	heap->end_address = heap->start_address + new_size;
}

static uint32_t contract(uint32_t new_size, heap_t * heap)
{
	if (new_size >= heap->end_address - heap->start_address)
	{
		return 0;
	}

	if (new_size & 0x1000)
	{
		new_size &= 0x1000;
		new_size += 0x1000;
	}

	if (new_size < KHEAP_MIN_SIZE)
	{
		new_size = KHEAP_MIN_SIZE;
	}

	uint32_t old_size = heap->end_address - heap->start_address;
	uint32_t i = old_size - 0x1000;

	while (new_size < i)
	{
		vmmngr_free_page(heap->start_address + i);
		i -= 0x1000;
	}

	heap->end_address = heap->start_address + new_size;

	return new_size;
}

void * alloc(uint32_t size, uint8_t page_align, heap_t * heap)
{
	uint32_t new_size = size + sizeof(header_t) + sizeof(footer_t);

	int32_t iterator = find_smallest_hole(new_size, page_align, heap);

	if ( iterator == -1)
	{
		uint32_t old_length = heap->end_address - heap->start_address;
		uint32_t old_end_address = heap->end_address;

		expand(old_length + new_size, heap);

		uint32_t new_length = heap->end_address - heap->start_address;

		iterator = 0;

		int32_t idx = -1;
		uint32_t value = 0x0;

		while ((uint32_t)iterator < heap->index.size)
		{
			uint32_t tmp = (uint32_t) lookup_ordered_array(iterator, &heap->index);
			if (tmp > value)
			{
				value = tmp;
				idx = iterator;
			}
			iterator++;
		}

		if (idx == -1)
		{
			header_t * header = (header_t *) old_end_address;
			header->magic = KHEAP_MAGIC;
			header->size = new_length - old_length;
			header->is_hole = 1;

			footer_t * footer = (footer_t *) (old_end_address + header->size - sizeof(footer_t));
			footer->header = header;
			footer->magic = KHEAP_MAGIC;
		}

		return alloc(size, page_align, heap);
	}

	header_t * orig_hole_header = (header_t *) lookup_ordered_array(iterator, &heap->index);
	uint32_t orig_hole_pos = (uint32_t) orig_hole_header;
	uint32_t orig_hole_size = orig_hole_header->size;

	if (orig_hole_size - new_size < (sizeof(header_t) + sizeof(footer_t)))
	{
		size += orig_hole_size - new_size;
		new_size = orig_hole_size;
	}

	if (page_align && (orig_hole_pos & 0xFFFFF000))
	{
		uint32_t new_location = orig_hole_pos + 0x1000 - (orig_hole_pos & 0xFFF) - sizeof(header_t);
		header_t * hole_header = (header_t *) orig_hole_pos;
		hole_header->size = 0x1000 - (orig_hole_pos & 0xFFF) - sizeof(header_t);
		hole_header->magic = KHEAP_MAGIC;
		hole_header->is_hole = 1;
		footer_t * hole_footer = (footer_t *) ((uint32_t) new_location - sizeof(footer_t));
		hole_footer->magic = KHEAP_MAGIC;
		hole_footer->header = hole_header;
		orig_hole_pos = new_location;
		orig_hole_size = orig_hole_size - hole_header->size;
	}
	else
	{
		remove_ordered_array(iterator, &heap->index);
	}

	header_t * block_header = (header_t *) orig_hole_pos;
	block_header->magic = KHEAP_MAGIC;
	block_header->is_hole = 0;
	block_header->size = new_size;

	footer_t * block_footer = (footer_t *) (orig_hole_pos + sizeof(header_t) + size);
	block_footer->magic = KHEAP_MAGIC;
	block_footer->header = block_header;

	if (orig_hole_size - new_size > 0)
	{
		header_t * hole_header = (header_t *) (orig_hole_pos + sizeof(header_t) + size + sizeof(footer_t));
		hole_header->magic = KHEAP_MAGIC;
		hole_header->is_hole = 1;
		hole_header->size = orig_hole_size - new_size;
		footer_t * hole_footer = (footer_t *) ((uint32_t) hole_header + orig_hole_size - new_size - sizeof(footer_t));
		if ((uint32_t) hole_footer < heap->end_address)
		{
			hole_footer->magic = KHEAP_MAGIC;
			hole_footer->header = hole_header;
		}

		insert_ordered_array((void *) hole_header, &heap->index);
	}

	return (void *) ((uint32_t) block_header + sizeof(header_t));
}

void free(void * p, heap_t * heap)
{
	if (p == 0)
	{
		return;
	}

	header_t * header = (header_t *) ((uint32_t) p - sizeof(header_t));
	footer_t * footer = (footer_t *) ((uint32_t) header + header->size - sizeof(footer_t));

	if (header->magic != footer->magic || header->magic != KHEAP_MAGIC)
	{
		return;
	}

	header->is_hole = 1;

	char do_add = 1;

	//Unify left
	footer_t * test_footer = (footer_t *) ((uint32_t) header - sizeof(footer_t));
	if (test_footer->magic == KHEAP_MAGIC && test_footer->header->is_hole == 1)
	{
		uint32_t cache_size = header->size;
		header = test_footer->header;
		footer->header = header;
		header->size += cache_size;
		do_add = 0;
	}

	//Unify right
	header_t * test_header = (header_t *) ((uint32_t) footer + sizeof(footer_t));
	if (test_header->magic == KHEAP_MAGIC && test_header->is_hole)
	{
		header->size += test_header->size;
		test_footer = (footer_t *) ((uint32_t) test_header + test_header->size - sizeof(footer_t));

		footer = test_footer;
		footer->header = header;

		uint32_t iterator = 0;
		while ((iterator < heap->index.size) &&
			(lookup_ordered_array(iterator, &heap->index) != (void *) test_header))
		{
			iterator++;
		}

		remove_ordered_array(iterator, &heap->index);

	}

	if ((uint32_t) footer + sizeof(footer_t) == heap->end_address)
	{
		uint32_t old_length = heap->end_address - heap->start_address;
		uint32_t new_length = contract((uint32_t) header - heap->start_address, heap);

		if (header->size - (old_length - new_length) > 0)
		{
			header->size -= old_length - new_length;
			footer = (footer_t *) ((uint32_t) header + header->size - sizeof(footer_t));
			footer->magic = KHEAP_MAGIC;
			footer->header =  header;
		}
		else
		{
			uint32_t iterator = 0;

			while((iterator < heap->index.size)
				&& lookup_ordered_array(iterator, &heap->index) != (void *) test_header)
			{
				iterator++;
			}

			if (iterator < heap->index.size)
			{
				remove_ordered_array(iterator, &heap->index);
			}
		}
	}

	if (do_add == 1)
	{
		insert_ordered_array((void *) header, &heap->index);
	}

}

void * do_kmalloc(uint32_t size, int align, uint32_t * phys)
{
	if (kheap != 0)
	{
		void * addr = alloc(size, (uint8_t) align, kheap);
		if (phys != 0)
		{
			ptable * ptable = vmmngr_get_ptable_address((virtual_addr)addr);
			pt_entry * pte = vmmngr_ptable_lookup_entry(ptable, (virtual_addr)addr);
			*phys = pt_entry_pfn(*pte) * 0x1000 + ((uint32_t) addr & 0xFFF);
		}

		return (void *) addr;
	}
	else
	{
		if (align == 1 && (placement_address & 0xFFFFF000))
		{
			placement_address &= 0xFFFFF000;
			placement_address += 0x1000;
		}

		if (phys)
		{
			*phys = placement_address;
		}

		uint32_t tmp = placement_address;
		placement_address += size;
		return (void*) tmp;
	}
}

void * kmalloc_align(uint32_t size)
{
	return do_kmalloc(size, 1, 0);
}
void * kmalloc(uint32_t size)
{
	return do_kmalloc(size, 0, 0);
}

int kmalloc_init(uint32_t kernel_end)
{
	placement_address = kernel_end;
	if (!vmmngr_alloc_page((virtual_addr) KHEAP_START))
	{
		printf("Couldn't allocate kheap start\n");
		return 0;
	}

	int i = 0; 
	do
	{
		vmmngr_alloc_page((virtual_addr) (KHEAP_START + i));
		i += 0x1000;
	} while ( i < KHEAP_INITIAL_SIZE);

	if (!vmmngr_alloc_page((virtual_addr)(KHEAP_START + KHEAP_INITIAL_SIZE - PAGE_SIZE)))
	{
		printf("Couldn't allocate kheap end\n");
		return 0;
	}

	kheap = create_heap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, 0x50000000, 0, 0);

	return 1;
}