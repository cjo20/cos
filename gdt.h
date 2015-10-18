#ifndef INCLUDE_GDT_H
#define INCLUDE_GDT_H

struct gdt_entry
{
	unsigned short limit_low;
	unsigned short base_low;
	unsigned char base_middle;
	unsigned char access;
	unsigned char granularity;
	unsigned char base_high;
} __attribute__((packed));

struct gdt_ptr
{
	unsigned short limit;
	struct gdt_entry * base;
} __attribute__((packed));

void gdt_install();
#endif