#include <stdio.h>

#include <utils/vsprintf.h>
#include <config.h>

int printf(const char *format, ...) {
	va_list args;
	char buf[1024] = {0};

	va_start(args, format);
	int tmp = vsprintf(buf, format, args);
	va_end(args);

	PRINTF_OUTPUT_FUNCTION(buf);

	return tmp;
}