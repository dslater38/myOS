/* vsprintf.c -- Lars Wirzenius & Linus Torvalds. */
/*
 * Wirzenius wrote this portably, Torvalds fucked it up :-)
 */

#include <stdarg.h>
#include <string.h>
#include "common.h"
#include "vesavga.h"

/* we use this so that we can do without the ctype library */
#define is_digit(c)	((c) >= '0' && (c) <= '9')


static int skip_atoi(const char **s)
{
	int i = 0;

	while(is_digit(**s))
		i = i * 10 + *((*s)++) - '0';
	return i;
}

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define SMALL	64		/* use 'abcdef' instead of 'ABCDEF' */

#define do_div(n,base) ({ \
long __res; \
__asm__("divq %4":"=a" (n),"=d" (__res):"0" (n),"1" (0),"r" (base)); \
__res; })

static char * number(char * str, long num, long base, int size, int precision
					 , int type)
{
	char sign = '\0', tmp[36];
	const char *digits =  (type&SMALL)  ? "0123456789abcdefghijklmnopqrstuvwxyz" : "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	if(type&LEFT)
	{
		type &= ~ZEROPAD;
	}

	if(base < 2 || base>36)
	{
		return 0;
	}

	char c = (type & ZEROPAD) ? '0' : ' ';
	if( (type&SIGN) && (num < 0) )
	{
		sign = '-';
		num = -num;
	}
	else if( type&PLUS )
	{
		sign = '+';
	}
	else if( type&SPACE )
	{
		sign = ' ';
	}

	if(sign)
	{
		size--;
	}

	if(type&SPECIAL)
	{
		if(base == 16)
		{
			size -= 2;
		}
		else if(base == 8)
		{
			size--;
		}
	}
	int i = 0;
	if(num == 0)
	{
		tmp[i++] = '0';
	}
	else
	{
		while(num != 0)
		{
			tmp[i++] = digits[do_div(num, base)];
		}
	}

	if(i > precision)
	{
		precision = i;
	}

	size -= precision;
	if(!(type&(ZEROPAD + LEFT)))
	{
		while(size-- > 0)
		{
			*str++ = ' ';
		}
	}

	if(sign)
	{
		*str++ = sign;
	}

	if(type&SPECIAL)
	{
		if(base == 8)
		{
			*str++ = '0';
		}
		else if(base == 16)
		{
			*str++ = '0';
			*str++ = digits[33];
		}
	}
	if(!(type&LEFT))
	{
		while(size-- > 0)
		{
			*str++ = c;
		}
	}

	while(i < precision--)
	{
		*str++ = '0';
	}

	while(i-- > 0)
	{
		*str++ = tmp[i];
	}

	while(size-- > 0)
	{
		*str++ = ' ';
	}

	return str;
}

static
unsigned long
get_integer(int qualifier, va_list args)
{
	unsigned long n = 0;

	switch(qualifier)
	{
		case 'l':
			n = static_cast<unsigned long>(va_arg(args, unsigned long));
			break;
		case 'h':
			n = static_cast<unsigned long>(va_arg(args, unsigned int));
			break;
		case (('l' << 8) | 'l'):
			n = static_cast<unsigned long>(va_arg(args, unsigned long long));
			break;
		case (('h' << 8) | 'h'):
			n = static_cast<unsigned long>(va_arg(args, unsigned int));
			break;
		default:
			n = va_arg(args, unsigned long);
			break;
	}

	return n;
}

static
int vsprintf_imp(char *buf, const char *fmt, va_list args)
{
//	size_t len=0;
	int i=0;
	char * str=nullptr;
// 	char *s=nullptr;
//	int *ip=nullptr;

//	int flags=0;		/* flags to number() */

// 	int field_width=0;	/* width of output field */
//	int precision=0;		/* min. # of digits for integers; max
//				   number of chars for from string */
//	int qualifier=0;		/* 'h', 'l', or 'L' for integer fields */

	for(str = buf; *fmt; ++fmt)
	{
		if(*fmt != '%')
		{
			*str++ = *fmt;
			continue;
		}

		/* process flags */
		int flags = 0;
	repeat:
		++fmt;		/* this also skips first '%' */
		switch(*fmt)
		{
		case '-': flags |= LEFT; goto repeat;
		case '+': flags |= PLUS; goto repeat;
		case ' ': flags |= SPACE; goto repeat;
		case '#': flags |= SPECIAL; goto repeat;
		case '0': flags |= ZEROPAD; goto repeat;
		}

		/* get field width */
		int field_width = -1;
		if(is_digit(*fmt))
		{
			field_width = skip_atoi(&fmt);
		}
		else if(*fmt == '*')
		{
			/* it's the next argument */
			field_width = va_arg(args, int);
			if(field_width < 0)
			{
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		int precision = -1;
		if(*fmt == '.')
		{
			++fmt;
			if(is_digit(*fmt))
				precision = skip_atoi(&fmt);
			else if(*fmt == '*')
			{
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if(precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		int qualifier = -1;
		if(*fmt == 'h' || *fmt == 'l' || *fmt == 'L')
		{
			qualifier = *fmt;
			++fmt;
			if(qualifier == 'h' && *fmt == 'h')
			{
				qualifier = (((qualifier & 0xFF) << 8) | 'h');
				++fmt;
			}
			else if(qualifier == 'l' && *fmt == 'l')
			{
				qualifier = (((qualifier & 0xFF) << 8) | 'l');
				++fmt;
			}
		}

		switch(*fmt)
		{
		case 'c':
			if(!(flags & LEFT))
			{
				while(--field_width > 0)
				{
					*str++ = ' ';
				}
			}

			*str++ = (unsigned char)va_arg(args, int);
			while(--field_width > 0)
			{
				*str++ = ' ';
			}
			break;

		case 's':
		{
			char *s = va_arg(args, char *);
			int len = static_cast<int>(strlen(s));
			if(precision < 0)
			{
				precision = len;
			}
			else if(len > precision)
			{
				len = precision;
			}

			if(!(flags & LEFT))
			{
				while(len < field_width--)
				{
					*str++ = ' ';
				}
			}
			for(i = 0; i < len; ++i)
			{
				*str++ = *s++;
			}
			while(len < field_width--)
			{
				*str++ = ' ';
			}
			break;
		}
		case 'o':
		{
			unsigned long n = get_integer(qualifier, args);
			str = number(str, n, 8, field_width, precision, flags);
			break;
		}
		case 'p':
			if(field_width == -1)
			{
				field_width = 16;
				flags |= ZEROPAD;
			}
			str = number(str,
				(unsigned long)va_arg(args, void *), 16,
						 field_width, precision, flags);
			break;

		case 'x':
			flags |= SMALL;
		case 'X':
		{
			unsigned long n = get_integer(qualifier, args);
			str = number(str, n, 16, field_width, precision, flags);
			break;
		}

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
		{
			unsigned long n = get_integer(qualifier, args);
			str = number(str, n, 10, field_width, precision, flags);
			break;
		}
		case 'n':
		{
			long *ip = va_arg(args, long *);
			*ip = (str - buf);
			break;
		}
		default:
			if(*fmt != '%')
			{
				*str++ = '%';
			}
			if(*fmt)
			{
				*str++ = *fmt;
			}
			else
			{
				--fmt;
			}
			break;
		}
	}
	*str = '\0';
	return static_cast<int>(str - buf);
}

extern "C"
{


	int vsprintf(char *buf, const char *fmt, va_list args)
	{
		return vsprintf_imp(buf, fmt, args);
	}

	int sprintf(char *buf, const char *fmt, ...)
	{
		int retVal = 0;
		va_list args;
		va_start(args, fmt);
		retVal = vsprintf_imp(buf, fmt, args);
		va_end(args);
		return retVal;
	}

	int printf(const char *fmt, ...)
	{
		char buf[4096] = { 0 };
		int retVal = 0;
		va_list args;
		va_start(args, fmt);
		retVal = vsprintf_imp(buf, fmt, args);
		va_end(args);
		monitor_write(buf);
		return retVal;
	}

}
