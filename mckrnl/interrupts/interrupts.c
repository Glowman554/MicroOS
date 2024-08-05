#include <interrupts/interrupts.h>

#include <gdb/gdb.h>

#include <interrupts/gdt.h>
#include <stdio.h>
#include <utils/io.h>
#include <scheduler/scheduler.h>

#include <driver/apic/lapic.h>

#include <driver/apic/smp.h>
#include <driver/acpi/madt.h>
#include <config.h>

#include <stddef.h>

long long unsigned int idt[IDT_ENTRIES];

static void idt_set_entry(int i, void (*fn)(), unsigned int selector, int flags) {
	debugf("Setting idt entry at %d with handler 0x%x and selector 0x%x and the flags 0x%x", i, (uint32_t) fn, selector, flags);

	unsigned long int handler = (unsigned long int) fn;
	idt[i] = handler & 0xffffLL;
	idt[i] |= (selector & 0xffffLL) << 16;
	idt[i] |= (flags & 0xffLL) << 40;
	idt[i] |= ((handler>> 16) & 0xffffLL) << 48;
}

void init_pic() {
	debugf("Initializing PIC");
	outb(0x20, 0x11);
	outb(0x21, 0x20);
	outb(0x21, 0x04);
	outb(0x21, 0x01);
	
	outb(0xa0, 0x11);
	outb(0xa1, 0x28);
	outb(0xa1, 0x02);
	outb(0xa1, 0x01);
	
	outb(0x21, 0x0);
	outb(0xa1, 0x0);
}

void init_interrupts() {
	idt_ptr idtp = {
		.limit = IDT_ENTRIES * 8 - 1,
		.pointer = idt,
	};

	init_pic();

	idt_set_entry(0, intr_stub_0, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(1, intr_stub_1, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(2, intr_stub_2, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(3, intr_stub_3, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(4, intr_stub_4, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(5, intr_stub_5, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(6, intr_stub_6, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(7, intr_stub_7, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(8, intr_stub_8, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(9, intr_stub_9, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(10, intr_stub_10, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(11, intr_stub_11, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(12, intr_stub_12, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(13, intr_stub_13, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(14, intr_stub_14, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(15, intr_stub_15, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(16, intr_stub_16, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(17, intr_stub_17, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(18, intr_stub_18, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);

	// IRQ-Handler
	idt_set_entry(32, intr_stub_32, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(33, intr_stub_33, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(34, intr_stub_34, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(35, intr_stub_35, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(36, intr_stub_36, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(37, intr_stub_37, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(38, intr_stub_38, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(39, intr_stub_39, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(40, intr_stub_40, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(41, intr_stub_41, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(42, intr_stub_42, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(43, intr_stub_43, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(44, intr_stub_44, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(45, intr_stub_45, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(46, intr_stub_46, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(47, intr_stub_47, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);

	// Syscall
	idt_set_entry(48, intr_stub_48, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING3 | IDT_FLAG_PRESENT);

	idt_set_entry(255, intr_stub_255, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	
	asm volatile("lidt %0" : : "m" (idtp));
	debugf("Enabling interrupts!");
	asm volatile("sti");
}


void halt_internal() {
	while(1) {
		asm volatile("cli; hlt");
	}
}

interrupt_handler_t interrupt_handlers[256] = { 0 };
void* interrupt_handlers_special_data[256] = { 0 };

void register_interrupt_handler(uint8_t interrupt_number, interrupt_handler_t handler, void* special_data) {
	debugf("Registering interrupt handler %p for interrupt %d", handler, interrupt_number);

	interrupt_handlers[interrupt_number] = handler;
	interrupt_handlers_special_data[interrupt_number] = special_data;
}

char* get_exception_name(int interrupt_number) {
	switch(interrupt_number) {
		case 0x0:
			return((char*) "Divide by Zero");
			break;
		case 0x1:
			return((char*) "Debug");
			break;
		case 0x2:
			return((char*) "Non Maskable Interrupt");
			break;
		case 0x3:
			return((char*) "Breakpoint");
			break;
		case 0x4:
			return((char*) "Overflow");
			break;
		case 0x5:
			return((char*) "Bound Range");
			break;
		case 0x6:
			return((char*) "Invalid Opcode");
			break;
		case 0x7:
			return((char*) "Device Not Available");
			break;
		case 0x8:
			return((char*) "Double Fault");
			break;
		case 0x9:
			return((char*) "Coprocessor Segment Overrun");
			break;
		case 0xa:
			return((char*) "Invalid TSS");
			break;
		case 0xb:
			return((char*) "Segment not Present");
			break;
		case 0xc:
			return((char*) "Stack Fault");
			break;
		case 0xd:
			return((char*) "General Protection");
			break;
		case 0xe:
			return((char*) "Page Fault");
			break;
		case 0x10:
			return((char*) "x87 Floating Point");
			break;
		case 0x11:
			return((char*) "Alignment Check");
			break;
		case 0x12:
			return((char*) "Machine Check");
			break;
		case 0x13:
			return((char*) "SIMD Floating Point");
			break;
		case 0x1e:
			return((char*) "Security-sensitive event in Host");
			break;
		default:
			return((char*) "Reserved");
			break;
	}
}


cpu_registers_t* handle_interrupt(cpu_registers_t* cpu) {
	// if (cpu->intr != 32) {
	// 	debugf("Interrupt: %d", cpu->intr);
	// }
	
	cpu_registers_t* new_cpu = cpu;

	if (cpu->intr == 0xff) {
		halt_internal();
	}

	if ((cpu->intr == 1 || cpu->intr == 3) && gdb_active) {
		gdb_interrupt(new_cpu);
		return new_cpu;
	}

	if (cpu->intr <= 0x1f) {
		if (interrupt_handlers[cpu->intr] != 0) {
			new_cpu = interrupt_handlers[cpu->intr](cpu, interrupt_handlers_special_data[cpu->intr]);
			set_tss(1, (uint32_t) (new_cpu + 1));
		} else {
			abortf("Unhandled exception %s (0x%x)", get_exception_name(cpu->intr), cpu->intr);
		}
	} else {
		if (cpu->intr >= 0x20 && cpu->intr <= 0x2f) {

			if (interrupt_handlers[cpu->intr] != 0) {
				new_cpu = interrupt_handlers[cpu->intr](cpu, interrupt_handlers_special_data[cpu->intr]);
				set_tss(1, (uint32_t) (new_cpu + 1));
			}

		#ifdef SMP
			LAPIC_ID(core);
			if (core != 0) { // TODO do not hard code
				lapic_eoi();
			} else {
				if (cpu->intr >= 0x28) {
					outb(0xa0, 0x20);
				}
				outb(0x20, 0x20);
			}
		
		#else
			if (cpu->intr >= 0x28) {
				outb(0xa0, 0x20);
			}
			outb(0x20, 0x20);
		#endif
		} else {
			if (interrupt_handlers[cpu->intr] != 0) {
				new_cpu = interrupt_handlers[cpu->intr](cpu, interrupt_handlers_special_data[cpu->intr]);
				set_tss(1, (uint32_t) (new_cpu + 1));
			}

			if (!get_self()->active) {
				return schedule(new_cpu, NULL);
			}
		}
	}

	return new_cpu;
}

void halt() {
#ifdef SMP
	LAPIC_ID(id);
	for (int i = 0; i < madt_lapic_count; i++) {
		if (i != id) {
			lapic_ipi(i, 0xff);
		}
	}
#endif

	halt_internal();
}