#ifndef INCLUDE_LIB_H
#define INCLUDE_LIB_H
int strlen(char * s);
void strrev(char *begin, char * end);
int strcmp(const char * s1, const char * s2);
unsigned long strtoul(const char * str, char ** endptr, int base);
void itoa(int value, char * buffer, int radix);
void utoa(unsigned int value, char * buffer, int radix);
int atoi(char * str);
#endif