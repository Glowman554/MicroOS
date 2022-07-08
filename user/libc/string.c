#include <string.h>

#include <stddef.h>
#include <stdio.h>

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


unsigned int __is_delim(char c, char* delim) {
	while(*delim != '\0') {
		if(c == *delim)
			return 1;
		delim++;
	}
	return 0;
}

char* strtok(char* src_string, char* delim) {
	static char* backup_string; // start of the next search

	if(!src_string) {
		src_string = backup_string;
	}

	if(!src_string) {
		// user is bad user
		return NULL;
	}

	// handle beginning of the string containing delims
	while(1) {
		if(__is_delim(*src_string, delim)) {
			src_string++;
			continue;
		}

		if(*src_string == '\0') {
			// we've reached the end of the string
			return NULL; 
		}
		break;
	}

	char* ret = src_string;
	while(1) {
		if(*src_string == '\0') {
			/*end of the input string and
			next exec will return NULL*/
			backup_string = src_string;
			return ret;
		}

		if(__is_delim(*src_string, delim)) {
			*src_string = '\0';
			backup_string = src_string + 1;
			return ret;
		}

		src_string++;
	}
}

int strncmp(char* str1, char* str2, int n) {
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