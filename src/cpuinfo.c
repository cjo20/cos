#include "cpuinfo.h"
#include "fb.h"

#define NUM_ECX_FEATURES 29
#define NUM_EDX_FEATURES 32

void print_vendor_string(int string[4])
{
	int i ,j;
	i = 1;
	for (j = 0; j < 4; ++j)
	{
		char c = (string[i] >> (8 * j)) & 0xFF;
		fb_write(&c, 1);
	}
	
	i = 3;
	for (j = 0; j < 4; ++j)
	{
		char c = (string[i] >> (8 * j)) & 0xFF;
		fb_write(&c, 1);
	}

	i = 2;
	for (j = 0; j < 4; ++j)
	{
		char c = (string[i] >> (8 * j)) & 0xFF;
		fb_write(&c, 1);
	}

	fb_writeString("\n");
}



char * ecx_features[32] = { 
							"SSE3   ",
							"PCLMUL ",
							"DTES64 ",
							"MONITOR",
							"DS_CPL ",
							"VMX    ",
							"SMX    ",
							"EST    ",
							"TM2    ",
							"SSSE3  ",
							"CID    ",
							"---    ",
							"FMA    ",
							"CX16   ",
							"ETPRD  ",
							"PDCM   ",
							"---    ",
							"---    ",
							"DCA    ",
							"SSE4_1 ",
							"SSE4_2 ",
							"x2APIC ",
							"MOVBE  ",
							"POPCNT ",
							"---    ",
							"AES    ",
							"XSAVE  ",
							"OSXSAVE",
							"AVX    ",
							"",
							"",
							""};

char * edx_features[32] = {
							"FPU    ",
							"VME    ",
							"DE     ",
							"PSE    ",
							"TSC    ",
							"MSR    ",
							"PAE    ",
							"MCE    ",
							"CX8    ",
							"APIC   ",
							"---    ",
							"SEP    ",
							"MTRR   ",
							"PGE    ",
							"MCA    ",
							"CMOV   ",
							"PAT    ",
							"PSE36  ",
							"PSN    ",
							"CLF    ",
							"---    ",
							"DTES   ",
							"ACPI   ",
							"MMX    ",
							"FXSR   ",
							"SSE    ",
							"SSE2   ",
							"SS     ",
							"HTT    ",
							"TM1    ",
							"IA64   ",
							"PBE    "};

void print_supported_features(int num_features, char * string[32], int field)
{
	int i = 0;
	for (i = 0; i < num_features; ++i)
	{
		fb_writeString(string[i]);
		fb_writeString(": ");
		if ((1 << i) & field)
		{
			fb_writeString("Y");
		}
		else
		{
			fb_writeString("N");
		}

		if ((i+1) % 6)
		{
			fb_writeString("   ");
		}
		else
		{
			fb_writeString("\n");
		}
	}
}

void parse_cpuid_features(int cpu_id[4])
{
	print_supported_features(NUM_ECX_FEATURES, ecx_features, cpu_id[2]);
	fb_writeString("\n");
	print_supported_features(NUM_EDX_FEATURES, edx_features, cpu_id[3]);

}