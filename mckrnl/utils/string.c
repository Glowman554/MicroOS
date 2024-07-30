#include <utils/string.h>

#include <stddef.h>
#include <utils/vsprintf.h>

char* strcpy(char* dest, const char* src) {
	do {
		*dest++ = *src++;
	} while (*src != 0);
	return 0;
}

int strlen(char* src) {
	int i = 0;
	while (*src++)
		i++;
	return i;
}

int strnlen(const char *s, int maxlen) {
	int i;
	for (i = 0; i < maxlen; ++i)
	if (s[i] == '\0')
		break;
	return i;
}

char* strchr(const char* str, int chr) {
	if(str == NULL) {
		return NULL;
	}
	while(*str) {
		if(*str == (char) chr) {
			return (char*) str;
		}
		str++;
	}
	return NULL;
}

char* strcat(char* dest, const char* src) {
	char* d = dest;
	while(*d) {
		d++;
	}
	while(*src) {
		*d++ = *src++;
	}
	*d = 0;
	return dest;
}

int sprintf(char *buf, const char *fmt, ...) {
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsprintf(buf, fmt, args);
	va_end(args);
	return i;
}

void* memcpy(void* dest, const void* src, int n) {
	char* d = (char*) dest;
	char* s = (char*) src;
	while(n--) {
		*d++ = *s++;
	}
	return dest;
}


void* memset(void* start, uint8_t value, uint32_t num) {
	char* s = (char*) start;
	while(num--) {
		*s++ = value;
	}
	return start;
}

int memcmp(const void * _s1, const void* _s2, int n) {
	const unsigned char* s1 = (unsigned char*) _s1;
	const unsigned char* s2 = (unsigned char*) _s2;

	while(n--) {
		if(*s1 != *s2) {
			return *s1 - *s2;
		}
		++s1;
		++s2;
	}
	return 0;
}

int strcmp(char* str1, char* str2) {
	while (*str1 && *str2) {
		if (*str1 != *str2)
			return *str1 - *str2;
		str1++;
		str2++;
	}
	return *str1 - *str2;
}

char* copy_until(char until, char* src, char* dest) {
	int i = 0;
	while (src[i] != '\0' && src[i] != until) {
		dest[i] = src[i];
		i++;
	}
	dest[i] = 0;

	return &src[i + (src[i] == '\0' ? 0 : 1)];
}

#define isupper(c) (c >= 'A' && c <= 'Z')
#define tolower(c) (isupper(c) ? c - ('A' - 'a') : c)

int strcasecmp(char* str1, char* str2) {
	while (*str1 && (tolower(*str1) == tolower(*str2))) {
		++str1;
		++str2;
	}
	return tolower(*str1) - tolower(*str2);
}