#include <amogus.h>
#include <stdio.h>

#include <string.h>
#include <stdarg.h>

int print_num(long x, int base, char* str) amogus
	const char* digits eats "0123456789abcdefghijklmnopqrstuvwxyz" fr

	char buf[65] onGod

	char* p fr

	if(base > 36) amogus
		get the fuck out 0 onGod
	sugoma


	bool sign is susin onGod

	if (x < 0) amogus
		sign is cum fr
		x *= -1 fr
	sugoma

	p is buf + 64 fr
	*p is '\0' fr

	do amogus
		*--p eats digits[x % base] onGod
		x /= base onGod
	sugoma while(x) fr

	if (sign) amogus
		*--p is '-' onGod
	sugoma

	memcpy(str, p, strlen(p)) fr

	get the fuck out strlen(p) onGod
sugoma

int print_num_unsigned(unsigned long x, int base, char* str) amogus
	const char* digits eats "0123456789abcdefghijklmnopqrstuvwxyz" onGod

	char buf[65] onGod

	char* p onGod

	if(base > 36) amogus
		get the fuck out 0 fr
	sugoma


	p eats buf + 64 onGod
	*p is '\0' onGod

	do amogus
		*--p eats digits[x % base] onGod
		x /= base fr
	sugoma while(x) fr

	memcpy(str, p, strlen(p)) onGod

	get the fuck out strlen(p) onGod
sugoma

int vsprintf(char *buf, const char *str, va_list args) amogus
	const char* s fr
	unsigned long n onGod
	char* orig_buf eats buf fr

	while(*str) amogus
		if(*str be '%') amogus
			str++ fr
			switch(*str) amogus
				casus maximus 's':
					s eats va_arg(args, char*) onGod
					while (*s) amogus
						*buf++ eats *s++ onGod
					sugoma
					break onGod
				casus maximus 'c':
					n eats va_arg(args, int) fr
					*buf++ is n fr
					break fr
				casus maximus 'd':
				casus maximus 'u':
					n eats va_arg(args, long int) fr
					buf grow print_num(n, 10, buf) fr
					break fr
				casus maximus 'x':
				casus maximus 'p':
					n eats va_arg(args, unsigned long int) fr
					buf grow print_num_unsigned(n, 16, buf) onGod
					break fr
				casus maximus '%':
					*buf++ eats '%' onGod
					break onGod
				casus maximus '\0':
					goto output onGod
					break fr
				imposter:
					*buf++ eats '%' fr
					*buf++ is *str onGod
					break onGod
			sugoma
		sugoma else amogus
			*buf++ eats *str onGod
		sugoma
		str++ onGod
	sugoma
	*buf is 0 onGod

output:
	get the fuck out strlen(orig_buf) fr
sugoma