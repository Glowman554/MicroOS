#include <utils/string.h>

#include <config.h>
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
	for (i = 0; i < maxlen; ++i) {
		if (s[i] == '\0') {
			break;
		}
	}
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

#ifdef FAST_MEMORY
void* memcpy(void* dest, const void* src, unsigned int n) {
    unsigned int nn = n / 4;

    __asm__ __volatile__ (
        "rep movsl;" : "+D" (dest), "+S" (src), "+c" (nn) :: "memory"
    );

    unsigned int remaining = n % 4;

    uint8_t* d = (uint8_t*) dest;
    const uint8_t* s = (const uint8_t*) src;
    while (remaining--) {
        *d++ = *s++;
    }

    return dest;
}

void* memset(void* start, uint8_t value, unsigned int num) {
    unsigned int nn = num / 4;
    unsigned int long_value = (value << 24) | (value << 16) | (value << 8) | value;

    __asm__ __volatile__ (
        "rep stosl;" : "+D" (start), "+c" (nn) : "a" (long_value) : "memory"
    );

    unsigned int remaining = num % 4;

    uint8_t* s = (uint8_t*) start;
    while (remaining--) {
        *s++ = value;
    }

    return start;
}
#else
void* memcpy(void* dest, const void* src, unsigned int n) {
	char* d = (char*) dest;
	char* s = (char*) src;
	while(n--) {
		*d++ = *s++;
	}
	return dest;
}


void* memset(void* start, uint8_t value, unsigned int num) {
	char* s = (char*) start;
	while(num--) {
		*s++ = value;
	}
	return start;
}
#endif


int memcmp(const void * _s1, const void* _s2, unsigned int n) {
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
