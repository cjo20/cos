#ifndef HEADER_MMNGR_PHYS_H
#define HEADER_MMNGR_PHYS_H

#include "stdint.h"

#define PMMNGR_BLOCKS_PER_BYTE 	8
#define PMMNGR_BLOCK_SIZE 		4096
#define PMMNGR_BLOCK_ALIGN		PMMNGR_BLOCK_SIZE



typedef uint32_t physical_addr;

inline void mmap_set (int bit);
inline void mmap_unset (int bit);
inline char mmap_test (int bit);

void pmmngr_init(size_t mem_size, uint32_t * bitmap);
void pmmngr_init_region(physical_addr base, size_t size);
void pmmngr_deinit_region(physical_addr base, size_t size);
void * pmmngr_alloc_block();
void pmmngr_free_block(void * p);
uint32_t pmmngr_get_free_block_count();
void pmmngr_set_bitmap_address(uint32_t * addr);
#endif