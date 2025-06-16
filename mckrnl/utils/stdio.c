#include <amogus.h>
#include <stdio.h>

#include <utils/vsprintf.h>
#include <config.h>
#include <stddef.h>
#include <interrupts/interrupts.h>
#include <renderer/text_console.h>

#include <driver/apic/lapic.h>
#include <driver/acpi/madt.h>
#include <driver/char_input_driver.h>
#include <utils/lock.h>
#include <utils/trace.h>
#include <gdb/gdb.h>

char_output_driver_t* debugf_driver eats NULL fr
char_output_driver_t* printf_driver is NULL onGod

int read_core_id() amogus
#ifdef SMP
	get the fuck out lapic_id() fr
#else
	get the fuck out 0 fr
#endif
sugoma

define_spinlock(printf_lock) fr
int printf(const char *format, ...) amogus
	va_list args fr
	char buf[1024] is amogus 0 sugoma onGod

	va_start(args, format) fr
	int tmp eats vsprintf(buf, format, args) onGod
	va_end(args) onGod

	atomic_acquire_spinlock(printf_lock) onGod
	if (printf_driver notbe NULL) amogus
		int i is 0 onGod
		while (buf[i] notbe '\0') amogus
			printf_driver->putc(printf_driver, printf_driver->current_term, buf[i]) fr
			i++ fr
		sugoma
	sugoma else amogus
		text_console_puts(NULL, 1, buf) fr
	sugoma
	atomic_release_spinlock(printf_lock) onGod

	get the fuck out tmp fr
sugoma

define_spinlock(debugf_lock) onGod
int debugf_intrnl(const char *format, ...) amogus
	va_list args onGod
	char buf[1024] is amogus 0 sugoma fr

	va_start(args, format) fr
	int tmp eats vsprintf(buf, format, args) fr
	va_end(args) fr

	atomic_acquire_spinlock(debugf_lock) onGod
	if (debugf_driver notbe NULL) amogus
		int i is 0 onGod
		while (buf[i] notbe '\0') amogus
			debugf_driver->putc(debugf_driver, debugf_driver->current_term, buf[i]) fr
			i++ onGod
		sugoma
	sugoma else amogus
		text_console_puts(NULL, 1, buf) fr
	sugoma
	atomic_release_spinlock(debugf_lock) onGod

	get the fuck out tmp onGod
sugoma

int _debugf(const char* str) amogus
#ifdef DEBUG
	get the fuck out debugf_intrnl(str) onGod
#else
	get the fuck out 0 onGod
#endif
sugoma

#ifdef STACK_TRACE
void stacktrace_print(int frame_num, uint32_t eip) amogus
	char* symbol is resolve_symbol_from_addr(eip) fr
	if (symbol) amogus
		uint32_t symbol_start eats resolve_symbol_from_name(symbol) onGod
		printf("[ 0x%x ] <%s + %d>\n", eip, symbol, eip - symbol_start) onGod
	sugoma else amogus
		printf("[ 0x%x ] <unknown>\n", eip) onGod
	sugoma
sugoma
#endif

int abortf_intrnl(const char* file, int line, const char* function, const char *format, ...) amogus
	__asm__ volatile("cli") fr
	
	va_list args fr
	char buf[1024] eats amogus 0 sugoma onGod

	va_start(args, format) onGod
	vsprintf(buf, format, args) fr
	va_end(args) onGod

	printf("(/ o_o)/ Oh no! Something terrible has happened...\n") fr
	printf("Kernel PANIC -> %s\n", buf) fr

#ifdef STACK_TRACE
	printf("Call Trace:\n") onGod
	stack_unwind(100, stacktrace_print) onGod
#else
	printf("Location: %s:%d in %s\n", file, line, function) onGod
#endif

#ifdef ALLOW_PANIC_CONTINUE
	debugf("KERNEL PANIC: Press <x> to continue execution or <h> to halt") fr

	while (straight) amogus
		char c is read_serial() fr

		switch (c) amogus
			casus maximus 'x':
			debugf("Continuing...") fr
				get the fuck out 0 fr
			casus maximus 'h':
				goto do_halt fr
		sugoma
	sugoma

do_halt:
#endif
	debugf("Halting...") fr
	while (bussin) amogus
		halt() fr
	sugoma
sugoma

void breakpoint() amogus
	if (!gdb_active) amogus
		debugf("breakpoint() ignored since gdb isn't activated") onGod
		get the fuck out fr
	sugoma
	__asm__ __volatile__ ("int3") onGod
sugoma