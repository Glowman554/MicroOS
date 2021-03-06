#include <stdio.h>

#include <utils/vsprintf.h>
#include <config.h>
#include <stddef.h>
#include <interrupts/interrupts.h>
#include <renderer/text_console.h>

char_output_driver_t* debugf_driver = NULL;
char_output_driver_t* printf_driver = NULL;

int printf(const char *format, ...) {
	va_list args;
	char buf[1024] = {0};

	va_start(args, format);
	int tmp = vsprintf(buf, format, args);
	va_end(args);

	if (printf_driver != NULL) {
		int i = 0;
		while (buf[i] != '\0') {
			printf_driver->putc(printf_driver, buf[i]);
			i++;
		}
	} else {
		text_console_puts(buf);
	}

	return tmp;
}

int debugf_intrnl(const char *format, ...) {
	va_list args;
	char buf[1024] = {0};

	va_start(args, format);
	int tmp = vsprintf(buf, format, args);
	va_end(args);

	if (debugf_driver != NULL) {
		int i = 0;
		while (buf[i] != '\0') {
			debugf_driver->putc(debugf_driver, buf[i]);
			i++;
		}
	} else {
		text_console_puts(buf);
	}

	return tmp;
}


int abortf(const char *format, ...) {
	va_list args;
	char buf[1024] = {0};

	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);

	printf("(/ o_o)/ Oh no! Something terrible has happened...\n");
	printf("Kernel PANIC -> %s\n", buf);

	while (true) {
		halt();
	}
}