#include <stdio.h>

#include <utils/vsprintf.h>
#include <config.h>
#include <stddef.h>
#include <interrupts/interrupts.h>
#include <renderer/text_console.h>

#include <driver/apic/lapic.h>
#include <driver/acpi/madt.h>
#include <utils/lock.h>
#include <utils/trace.h>

char_output_driver_t* debugf_driver = NULL;
char_output_driver_t* printf_driver = NULL;

int read_core_id() {
#ifdef SMP
	LAPIC_ID(id);
	return id;
#else
	return 0;
#endif
}

define_spinlock(printf_lock);
int printf(const char *format, ...) {
	va_list args;
	char buf[1024] = {0};

	va_start(args, format);
	int tmp = vsprintf(buf, format, args);
	va_end(args);

	atomic_acquire_spinlock(printf_lock);
	if (printf_driver != NULL) {
		int i = 0;
		while (buf[i] != '\0') {
			printf_driver->putc(printf_driver, buf[i]);
			i++;
		}
	} else {
		text_console_puts(buf);
	}
	atomic_release_spinlock(printf_lock);

	return tmp;
}

define_spinlock(debugf_lock);
int debugf_intrnl(const char *format, ...) {
	va_list args;
	char buf[1024] = {0};

	va_start(args, format);
	int tmp = vsprintf(buf, format, args);
	va_end(args);

	atomic_acquire_spinlock(debugf_lock);
	if (debugf_driver != NULL) {
		int i = 0;
		while (buf[i] != '\0') {
			debugf_driver->putc(debugf_driver, buf[i]);
			i++;
		}
	} else {
		text_console_puts(buf);
	}
	atomic_release_spinlock(debugf_lock);

	return tmp;
}

int _debugf(const char* str) {
#ifdef DEBUG
	return debugf_intrnl(str);
#else
	return 0;
#endif
}

void stacktrace_print(int frame_num, uint32_t eip) {
	printf("[ %d 0x%x ]\n", frame_num, eip);
	debugf("[ %d 0x%x ]", frame_num, eip);
}

int abortf(const char *format, ...) {
	__asm__ volatile("cli");
	
	va_list args;
	char buf[1024] = {0};

	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);

	printf("(/ o_o)/ Oh no! Something terrible has happened...\n");
	printf("Kernel PANIC -> %s\n", buf);

	printf("Call Trace:\n");
	stack_unwind(100, stacktrace_print);

	while (true) {
		halt();
	}
}