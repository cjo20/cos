#include "lib.h"
#include "stdint.h"
#include "fb.h"
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

inline int isspace(int c)
{
	return (c == ' '
		 || c == '\t'
		 || c == '\n'
		 || c == '\12');
}

inline int isdigit(int c)
{
	return (c >= '0' && c <= '9');
		 
}

inline int isalpha(int c)
{
	return ((c >= 'a' && c <= 'z') 
		  || (c >= 'A' && c <= 'Z'));
}

inline int isupper(int c)
{
	return (c >= 'A' && c <= 'Z');
}

unsigned long strtoul(const char * str, char ** endptr, int base)
{
	unsigned char c;

	const char * inp = str;
	do 
	{
		c = *inp++;
	} while(isspace(c));
	
	if ((base == 0 || base == 2)
		&& c == '0' 
		&& (*inp == 'b' || *inp == 'B'))
	{
		inp++; //skip x
		c = *inp++;
		base = 2;
	}
	else if ((base == 0 || base == 16)
		&& c == '0' 
		&& (*inp == 'x' || *inp == 'X'))
	{
		inp++; //skip x
		c = *inp++;
		base = 16;
	}
	else if (base == 0)
	{
		if (c == '0')
		{
			base = 8;
		}
		else
		{
			base = 10;
		}
	}
	
	unsigned long largestValue = 0;
	unsigned long cutlim = 0;
	unsigned long cutoff = 0;
	unsigned long result = 0;
	char any;


	largestValue = 0xFFFFFFFF;
	cutlim = largestValue % (unsigned long) base;
	cutoff = largestValue / (unsigned long) base;

	for (result = 0, any = 0; ; c = *inp++)
	{
		if (isdigit(c))
		{
			c -= '0';
		}
		else if (isalpha(c))
		{
			if (isupper(c))
			{
				c -= 'A';
				c += 10;
			}
			else
			{
				c -= 'a';
				c += 10;
			}
		}
		else
		{
			break;
		}

		if (c >= base)
		{
			break;
		}

		if (any < 0 
		 || result > cutoff 
		 || (result == cutoff && c > cutlim))
		{
			any = -1;
		}
		else
		{
			any = 1;
			result *= base;
			result += c;
		}
	}

	if (any < 0)
	{
			result = 0xFFFFFFFF;
	}

	if (endptr != 0)
	{
		*endptr = (char *) (any ? inp - 1 : str);
	}

	return result;

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