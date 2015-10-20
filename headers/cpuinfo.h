#ifndef INCLUDE_CPUINFO_H
#define INCLUDE_CPUINFO_H
void print_vendor_string(int string[4]);
void parse_cpuid_features(int cpu_id[4]);
#endif