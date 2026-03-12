#include <string.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

char* strcpy(char* dest, const char* src) {
	do {
		*dest++ = *src++;
	} while (*src != 0);
	return 0;
}

size_t strlen(const char* s) {
	size_t i = 0;
	while (*s++) {
		i++;
	}
	return i;
}

size_t strnlen(const char* s, size_t maxlen) {
	size_t i;
	for (i = 0; i < maxlen; ++i) {
		if (s[i] == '\0') {
			break;
		}
	}
	return i;
}

char* strchr(const char* str, int chr) {
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

void* memcpy(void* dest, const void* src, size_t n) {
	char* d = (char*) dest;
	char* s = (char*) src;
	while(n--) {
		*d++ = *s++;
	}
	return dest;
}

void* memset(void* s, int c, size_t n) {
	unsigned char* p = (unsigned char*) s;
	while(n--) {
		*p++ = (unsigned char) c;
	}
	return s;
}

int memcmp(const void* s1, const void* s2, size_t n) {
	const unsigned char* s1c = (const unsigned char*) s1;
	const unsigned char* s2c = (const unsigned char*) s2;

	while(n--) {
		if(*s1c != *s2c) {
			return *s1c - *s2c;
		}
		++s1c;
		++s2c;
	}
	return 0;
}

void* memmove(void* dest, const void* src, size_t n) {
	char* d = (char*) dest;
	char* s = (char*) src;
	if(d < s) {
		while(n--) {
			*d++ = *s++;
		}
	} else {
		d += n;
		s += n;
		while(n--) {
			*--d = *--s;
		}
	}
	return dest;
}

int strcmp(const char* str1, const char* str2) {
	while (*str1 && *str2) {
		if (*str1 != *str2)
			return *str1 - *str2;
		str1++;
		str2++;
	}
	return *str1 - *str2;
}


unsigned int __is_delim(char c, const char* delim) {
	while(*delim != '\0') {
		if(c == *delim)
			return 1;
		delim++;
	}
	return 0;
}

char* strtok(char* str, const char* delim) {
	static char* backup_string; // start of the next search

	if(!str) {
		str = backup_string;
	}

	if(!str) {
		// user is bad user
		return NULL;
	}

	// handle beginning of the string containing delims
	while(1) {
		if(__is_delim(*str, delim)) {
			str++;
			continue;
		}

		if(*str == '\0') {
			// we've reached the end of the string
			return NULL; 
		}
		break;
	}

	char* ret = str;
	while(1) {
		if(*str == '\0') {
			/*end of the input string and
			next exec will return NULL*/
			backup_string = str;
			return ret;
		}

		if(__is_delim(*str, delim)) {
			*str = '\0';
			backup_string = str + 1;
			return ret;
		}

		str++;
	}
}

int strncmp(const char* str1, const char* str2, size_t n) {
	while (n && *str1 && (*str1 == *str2)) {
		++str1;
		++str2;
		--n;
	}
	if (n == 0) {
		return 0;
	} else {
		return *str1 - *str2;
	}
}

char* strndup(const char* str, size_t n) {
    size_t len = strlen(str);
    size_t copy_len = len < n ? len : n;
    char* new_str = malloc(copy_len + 1);
    if (new_str) {
        memcpy(new_str, str, copy_len);
        new_str[copy_len] = '\0';
    }
    return new_str;
}

char* strstr(const char* haystack, const char* needle) {
	if (*needle == '\0') {
        return (char*) haystack;
    }

    while (*haystack) {
        const char* h = haystack;
        const char* n = needle;
        
        while (*n && *h == *n) {
            h++;
            n++;
        }

        if (*n == '\0') {
            return (char*)haystack;
        }

        haystack++;
    }

    return NULL;
}

char* strdup(const char* str) {
	size_t len = strlen(str) + 1;
	char* dup = malloc(len);
	memset(dup, 0, len);
    strcpy(dup, str);

    return dup;
}