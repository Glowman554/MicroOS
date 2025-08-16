#include <stdio.h>
#include <stddef.h>
#include <sys/file.h>
#include <string.h>

int printf(const char *fmt, ...) {
	va_list args;
	char buf[1024] = {0};

	va_start(args, fmt);
	int tmp = vsprintf(buf, fmt, args);
	va_end(args);

	write(STDOUT, buf, strlen(buf), 0);

	return tmp;
}

int puts(const char* s) {
	int l = strlen(s);

	write(STDOUT, (char*) s, l, 0);

	return l;
}