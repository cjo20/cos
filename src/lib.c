#include "lib.h"


void strrev(char *begin, char * end)
{
	char aux;

	while (end > begin)
	{
		aux = *end;
		*end = *begin;
		end--;

		*begin = aux;
		begin++;
	}
}

int strcmp(const char * s1, const char * s2)
{
	unsigned char c1;
	unsigned char c2;
	while (*s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}

	c1 = *((unsigned char *) s1);
	c2 = *((unsigned char *) s2);

	return c1 - c2;
}

int atoi(char * str)
{
	int res;
	int sign = 1;

	if (*str == '-')
	{
		sign = -1;
		str++;
	}
	while(*str >= '0' && *str <= '9')
	{
		res *= 10;
		res += *str - '0';
		str++;
	}

	return res * sign;
}

void itoa(int value, char * str, int base)
{
	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char * wstr = str;

	int sign;

	if (base < 2 || base > 35)
	{
		*wstr = '\0';
		return;
	}

	if ((sign=value) < 0)
	{
		value = -value;
	}	

	do 
	{
		*wstr++ = num[value % base];
	} while (value /= base);

	if (sign < 0)
	{
		*wstr++ = '-';
	}

	*wstr='\0';

	strrev(str, wstr-1);
}

void utoa(unsigned int value, char * str, int base)
{
	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char * wstr = str;

	if (base < 2 || base > 35)
	{
		*wstr = '\0';
		return;
	}

	do 
	{
		*wstr++ = num[value % base];
	} while (value /= base);

	*wstr='\0';

	strrev(str, wstr-1);
}

int strlen(char * s)
{
	int i = 0;

	while(*s != 0)
	{
		i++;
		s++;
	}

	return i;
}