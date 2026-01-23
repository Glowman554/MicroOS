#include <stdio.h>

#include <utils/vsprintf.h>
#include <config.h>
#include <stddef.h>
#include <interrupts/interrupts.h>
#include <scheduler/scheduler.h>
#include <scheduler/killer.h>
#include <renderer/text_console.h>

#include <driver/apic/lapic.h>
#include <driver/acpi/madt.h>
#include <driver/char_input_driver.h>
#include <utils/lock.h>
#include <utils/trace.h>
#include <gdb/gdb.h>

#include <module.h>

char_output_driver_t* debugf_driver = NULL;
char_output_driver_t* printf_driver = NULL;

int read_core_id() {
#ifdef SMP
	return lapic_id();
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
			printf_driver->putc(printf_driver, printf_driver->current_term, buf[i]);
			i++;
		}
	} else {
		text_console_puts(NULL, 1, buf);
	}
	atomic_release_spinlock(printf_lock);

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
			debugf_driver->putc(debugf_driver, debugf_driver->current_term, buf[i]);
			i++;
		}
	} else {
		text_console_puts(NULL, 1, buf);
	}

	return tmp;
}

int _debugf(const char* str) {
#ifdef DEBUG
	return debugf_intrnl(str);
#else
	return 0;
#endif
}

#ifdef STACK_TRACE
void stacktrace_print(int frame_num, uint32_t eip) {
	char* symbol = resolve_symbol_from_addr(eip);
	if (symbol) {
		uint32_t symbol_start = resolve_symbol_from_name(symbol);
		printf("[ 0x%x ] <%s + %d>\n", eip, symbol, eip - symbol_start);
	} else {
		for (int i = 0; i < loaded_module_count; i++) {
			module_t* module = loaded_modules[i];
			char* mod_symbol = resolve_symbol_name(&module->ctx, (void*)eip);
			if (mod_symbol) {
				uint32_t symbol_start = (uint32_t) resolve_symbol_address(&module->ctx, mod_symbol);
				printf("[ 0x%x (%s) ] <%s + %d>\n", eip, module->name, mod_symbol, eip - symbol_start);
				return;
			}
		}

		printf("[ 0x%x ] <unknown>\n", eip);
	}
}
#endif

int abortf_intrnl(const char* file, int line, const char* function, bool weak, const char *format, ...) {
	__asm__ volatile("cli");
	
	va_list args;
	char buf[1024] = {0};

	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);

	printf("(/ o_o)/ Oh no! Something terrible has happened...\n");
	printf("Kernel PANIC -> %s\n", buf);

#ifdef STACK_TRACE
	printf("Call Trace:\n");
	stack_unwind(100, stacktrace_print);
#else
	printf("Location: %s:%d in %s\n", file, line, function);
#endif

	if (is_scheduler_running && weak) {
		kill("Kernel Panic");
	} else {
	#ifdef ALLOW_PANIC_CONTINUE
		debugf("KERNEL PANIC: Press <x> to continue execution or <h> to halt");

		while (true) {
			char c = read_serial();

			switch (c) {
				case 'x':
				debugf("Continuing...");
					return 0;
				case 'h':
					goto do_halt;
			}
		}
	#endif
	}
do_halt:
	debugf("Halting...");
	while (true) {
		halt();
	}

}

void breakpoint() {
	if (!gdb_active) {
		debugf("breakpoint() ignored since gdb isn't activated");
		return;
	}
	__asm__ __volatile__ ("int3");
}