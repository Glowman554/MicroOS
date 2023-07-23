#include <strings.h>
#include <ctype.h>

int strncasecmp(char* str1, char* str2, int n) {
	while (n && *str1 && (tolower(*str1) == tolower(*str2))) {
		++str1;
		++str2;
		--n;
	}
	if (n == 0) {
		return 0;
	} else {
		return tolower(*str1) - tolower(*str2);
	}
}