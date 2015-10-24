#ifndef INCLUDE_KMALLOC_H
#define INCLUDE_KMALLOC_H

#include "stdint.h"
#include "ordered_array.h"

#define KHEAP_START 		0x20000000
#define KHEAP_INITIAL_SIZE 	0x100000
#define KHEAP_INDEX_SIZE	0x20000
#define KHEAP_MAGIC 		0x123ABC4F
#define KHEAP_MIN_SIZE		0x70000

typedef struct
{
	uint32_t magic;
	uint8_t is_hole;
	uint32_t size;
} header_t;

typedef struct 
{
	uint32_t magic;
	header_t *header;
} footer_t;

void * kmalloc(uint32_t size);

typedef struct 
{
	ordered_array_t index;
	uint32_t 	start_address;
	uint32_t 	end_address;
	uint32_t 	max_address;
	uint8_t 	supervisor;
	uint8_t		readonly;
}heap_t;

heap_t * create_heap(uint32_t start, uint32_t end, uint32_t max, uint8_t supervisor, uint8_t readonly);

void * alloc(uint32_t size, uint8_t page_align, heap_t * heap);
void free(void * p, heap_t * heap);
int kmalloc_init(uint32_t kernel_end);

#endif