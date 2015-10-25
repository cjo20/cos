#ifndef INCLUDE_MEM_H
#define INCLUDE_MEM_H
#include "stdint.h"
void memsetd(void * destination, uint32_t val, unsigned int size);
void * memcpy(void * destination, const void * source, unsigned int num);
void memset(void * destination, char val, unsigned int size);
#endif