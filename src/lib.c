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