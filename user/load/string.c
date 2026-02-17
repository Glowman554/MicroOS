#include <string.h>

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

int strcmp(const char* str1, const char* str2) {
	while (*str1 && *str2) {
		if (*str1 != *str2)
			return *str1 - *str2;
		str1++;
		str2++;
	}
	return *str1 - *str2;
}


int strlen(const char* src) {
	int i = 0;
	while (*src++)
		i++;
	return i;
}
