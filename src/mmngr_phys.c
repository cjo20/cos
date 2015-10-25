#include "mmngr_phys.h"
#include "mem.h"
// size of physical memory
static uint32_t _mmngr_memory_size = 0;

// number of blocks currently in use
static uint32_t _mmngr_used_blocks = 0;

// maximum number of blocks
static uint32_t _mmngr_max_blocks = 0;

// memory map bit array
static uint32_t *_mmngr_memory_map = 0;


/* pmmngr_get_block_count
*	Returns total number of blocks being managed
**/
uint32_t pmmngr_get_block_count()
{
	return _mmngr_max_blocks;
}

uint32_t pmmngr_get_free_block_count()
{
	return _mmngr_max_blocks - _mmngr_used_blocks;
}

inline void mmap_set (int bit)
{
	_mmngr_memory_map[bit >> 5] |= (1 << (bit % 32));
}

inline void mmap_unset (int bit)
{
	_mmngr_memory_map[bit >> 5] &= ~ (1 << (bit % 32));
}

inline char mmap_test (int bit)
{
	return _mmngr_memory_map[bit >> 5] & (1 << (bit % 32));
}

int mmap_first_free()
{
	uint32_t i = 0;
	uint32_t j = 0;

	for (i = 0; i < pmmngr_get_block_count() / 32; ++i)
	{
		if (_mmngr_memory_map[i] != 0xffffffff)
		{
			for (j = 0; j < 32; ++j)
			{
				int bit = 1 << j;
				if (!(_mmngr_memory_map[i] & bit))
				{
					return (i * sizeof(uint32_t) * PMMNGR_BLOCKS_PER_BYTE) + j;
				}
			}
		}
	}

	return -1;
}



/* pmmngr_init:
*	Initialises the physical memory manager
*
*	@param mem_size 	Size of physical memory in kB
*	@param bitmap		Location to store memory map structure
*/
void pmmngr_init(size_t mem_size, uint32_t * bitmap)
{
	_mmngr_memory_size = mem_size;
	_mmngr_memory_map = bitmap;
	_mmngr_max_blocks = (mem_size * 1024) / PMMNGR_BLOCK_SIZE;
	_mmngr_used_blocks = pmmngr_get_block_count();

	uint32_t size = pmmngr_get_block_count() / PMMNGR_BLOCKS_PER_BYTE;
	if (size & 0x3)
	{
		memset(_mmngr_memory_map, 0xff, size);
	}
	else
	{
		memsetd(_mmngr_memory_map, 0xff, size >> 2);	
	}
}

void pmmngr_init_region(physical_addr base, size_t size)
{
	int align = base / PMMNGR_BLOCK_SIZE;
	int blocks = size / PMMNGR_BLOCK_SIZE;

	for (; blocks > 0; blocks--)
	{
		mmap_unset(align++);
		_mmngr_used_blocks--;
	}

	mmap_set(0);
}

void pmmngr_deinit_region(physical_addr base, size_t size)
{
	int align = base / PMMNGR_BLOCK_SIZE;
	int blocks = size / PMMNGR_BLOCK_SIZE;

	for(; blocks > 0; blocks--)
	{
		mmap_set(align++);
		_mmngr_used_blocks++;
	}
}

void * pmmngr_alloc_block()
{
	if (pmmngr_get_free_block_count() <= 0)
	{
		return 0;
	}

	int frame = mmap_first_free();

	if (frame == -1)
	{
		return 0;
	}

	mmap_set(frame);

	physical_addr addr = frame * PMMNGR_BLOCK_SIZE;
	_mmngr_used_blocks++;

	return (void *) addr;
}

void pmmngr_free_block(void * p)
{
	physical_addr addr = (physical_addr) p;
	int frame = addr / PMMNGR_BLOCK_SIZE;

	mmap_unset(frame);
	_mmngr_used_blocks--;
}

void pmmngr_set_bitmap_address(uint32_t * addr)
{
	_mmngr_memory_map = addr;
}