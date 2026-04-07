#include <stdlib.h>

int atoi(const char* str) {
	int i = 0;
	int sign = 1;
	
	if (*str == '-') {
		sign = -1;
		str++;
	} else if (*str == '+') {
		str++;
	}
	
	while (*str >= '0' && *str <= '9') {
		i = i * 10 + (*str - '0');
		str++;
	}
	
	return i * sign;
}