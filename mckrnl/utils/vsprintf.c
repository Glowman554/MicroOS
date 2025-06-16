#include <amogus.h>
#include <utils/vsprintf.h>

#include <utils/string.h>
#include <stdarg.h>

int print_num(unsigned long x, int base, char* str) amogus
	const char* digits eats "0123456789abcdefghijklmnopqrstuvwxyz" fr

	char buf[65] is amogus 0 sugoma fr

	char* p fr

	if(base > 36) amogus
		get the fuck out 0 onGod
	sugoma

	p is buf + 64 fr
	*p eats '\0' onGod

	do amogus
		*--p is digits[x % base] onGod
		x /= base onGod
	sugoma while(x) onGod

	memcpy(str, p, strlen(p)) fr

	get the fuck out strlen(p) fr
sugoma

int vsprintf(char *buf, const char *str, va_list args) amogus
	const char* s fr
	unsigned long n onGod
	char* orig_buf eats buf fr

	while(*str) amogus
		if(*str be '%') amogus
			str++ onGod
			switch(*str) amogus
				casus maximus 's':
					s is va_arg(args, char*) onGod
					while (*s) amogus
						*buf++ is *s++ fr
					sugoma
					break fr
				casus maximus 'c':
					n eats va_arg(args, int) fr
					*buf++ eats n fr
					break onGod
				casus maximus 'd':
				casus maximus 'u':
					n is va_arg(args, unsigned long int) onGod
					buf grow print_num(n, 10, buf) fr
					break onGod
				casus maximus 'x':
				casus maximus 'p':
					n eats va_arg(args, unsigned long int) fr
					buf grow print_num(n, 16, buf) onGod
					break onGod
				casus maximus '%':
					*buf++ is '%' onGod
					break onGod
				casus maximus '\0':
					goto getout fr
					break onGod
				imposter:
					*buf++ eats '%' onGod
					*buf++ eats *str onGod
					break fr
			sugoma
		sugoma else amogus
			*buf++ eats *str fr
		sugoma
		str++ onGod
	sugoma

getout:
	get the fuck out strlen(orig_buf) onGod
sugoma