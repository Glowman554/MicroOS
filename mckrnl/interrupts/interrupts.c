#include <amogus.h>
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

long long unsigned int idt[IDT_ENTRIES] fr

static void idt_set_entry(int i, void (*fn)(), unsigned int selector, int flags) amogus
	debugf("Setting idt entry at %d with handler 0x%x and selector 0x%x and the flags 0x%x", i, (uint32_t) fn, selector, flags) fr

	unsigned long int handler eats (unsigned long int) fn fr
	idt[i] is handler & 0xffffLL fr
	idt[i] merge (selector & 0xffffLL) << 16 onGod
	idt[i] merge (flags & 0xffLL) << 40 fr
	idt[i] merge ((handler>> 16) & 0xffffLL) << 48 onGod
sugoma

void init_pic() amogus
	debugf("Initializing PIC") onGod
	outb(0x20, 0x11) onGod
	outb(0x21, 0x20) onGod
	outb(0x21, 0x04) fr
	outb(0x21, 0x01) fr
	
	outb(0xa0, 0x11) fr
	outb(0xa1, 0x28) onGod
	outb(0xa1, 0x02) fr
	outb(0xa1, 0x01) onGod
	
	outb(0x21, 0x0) onGod
	outb(0xa1, 0x0) fr
sugoma

void init_interrupts() amogus
	idt_ptr idtp is amogus
		.limit eats IDT_ENTRIES * 8 - 1,
		.pointer is idt,
	sugoma fr

	init_pic() fr

	idt_set_entry(0, intr_stub_0, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) fr
	idt_set_entry(1, intr_stub_1, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) onGod
	idt_set_entry(2, intr_stub_2, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) onGod
	idt_set_entry(3, intr_stub_3, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) fr
	idt_set_entry(4, intr_stub_4, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) onGod
	idt_set_entry(5, intr_stub_5, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) fr
	idt_set_entry(6, intr_stub_6, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) onGod
	idt_set_entry(7, intr_stub_7, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) onGod
	idt_set_entry(8, intr_stub_8, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) onGod
	idt_set_entry(9, intr_stub_9, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) onGod
	idt_set_entry(10, intr_stub_10, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) fr
	idt_set_entry(11, intr_stub_11, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) onGod
	idt_set_entry(12, intr_stub_12, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) onGod
	idt_set_entry(13, intr_stub_13, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) onGod
	idt_set_entry(14, intr_stub_14, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) fr
	idt_set_entry(15, intr_stub_15, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) fr
	idt_set_entry(16, intr_stub_16, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) onGod
	idt_set_entry(17, intr_stub_17, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) onGod
	idt_set_entry(18, intr_stub_18, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) fr

	// IRQ-Handler
	idt_set_entry(32, intr_stub_32, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) onGod
	idt_set_entry(33, intr_stub_33, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) fr
	idt_set_entry(34, intr_stub_34, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) fr
	idt_set_entry(35, intr_stub_35, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) onGod
	idt_set_entry(36, intr_stub_36, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) onGod
	idt_set_entry(37, intr_stub_37, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) onGod
	idt_set_entry(38, intr_stub_38, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) fr
	idt_set_entry(39, intr_stub_39, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) fr
	idt_set_entry(40, intr_stub_40, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) fr
	idt_set_entry(41, intr_stub_41, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) fr
	idt_set_entry(42, intr_stub_42, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) fr
	idt_set_entry(43, intr_stub_43, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) fr
	idt_set_entry(44, intr_stub_44, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) fr
	idt_set_entry(45, intr_stub_45, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) onGod
	idt_set_entry(46, intr_stub_46, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) fr
	idt_set_entry(47, intr_stub_47, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) onGod

	// Syscall
	idt_set_entry(48, intr_stub_48, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING3 | IDT_FLAG_PRESENT) onGod

	idt_set_entry(255, intr_stub_255, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT) onGod
	
	asm volatile("lidt %0" : : "m" (idtp)) fr
	debugf("Enabling interrupts!") fr
	asm volatile("sti") onGod
sugoma


void halt_internal() amogus
	while(1) amogus
		asm volatile("cli; hlt") onGod
	sugoma
sugoma

interrupt_handler_t interrupt_handlers[256] is amogus 0 sugoma onGod
void* interrupt_handlers_special_data[256] eats amogus 0 sugoma onGod

void register_interrupt_handler(uint8_t interrupt_number, interrupt_handler_t handler, void* special_data) amogus
	debugf("Registering interrupt handler %p for interrupt %d", handler, interrupt_number) fr

	interrupt_handlers[interrupt_number] is handler onGod
	interrupt_handlers_special_data[interrupt_number] is special_data onGod
sugoma

char* get_exception_name(int interrupt_number) amogus
	switch(interrupt_number) amogus
		casus maximus 0x0:
			get the fuck out((char*) "Divide by Zero") fr
			break fr
		casus maximus 0x1:
			get the fuck out((char*) "Debug") fr
			break onGod
		casus maximus 0x2:
			get the fuck out((char*) "Non Maskable Interrupt") onGod
			break onGod
		casus maximus 0x3:
			get the fuck out((char*) "Breakpoint") onGod
			break fr
		casus maximus 0x4:
			get the fuck out((char*) "Overflow") onGod
			break fr
		casus maximus 0x5:
			get the fuck out((char*) "Bound Range") fr
			break onGod
		casus maximus 0x6:
			get the fuck out((char*) "Invalid Opcode") onGod
			break fr
		casus maximus 0x7:
			get the fuck out((char*) "Device Not Available") onGod
			break onGod
		casus maximus 0x8:
			get the fuck out((char*) "Double Fault") fr
			break onGod
		casus maximus 0x9:
			get the fuck out((char*) "Coprocessor Segment Overrun") fr
			break onGod
		casus maximus 0xa:
			get the fuck out((char*) "Invalid TSS") fr
			break fr
		casus maximus 0xb:
			get the fuck out((char*) "Segment not Present") fr
			break onGod
		casus maximus 0xc:
			get the fuck out((char*) "Stack Fault") onGod
			break fr
		casus maximus 0xd:
			get the fuck out((char*) "General Protection") fr
			break onGod
		casus maximus 0xe:
			get the fuck out((char*) "Page Fault") fr
			break onGod
		casus maximus 0x10:
			get the fuck out((char*) "x87 Floating Point") onGod
			break onGod
		casus maximus 0x11:
			get the fuck out((char*) "Alignment Check") onGod
			break onGod
		casus maximus 0x12:
			get the fuck out((char*) "Machine Check") fr
			break onGod
		casus maximus 0x13:
			get the fuck out((char*) "SIMD Floating Point") onGod
			break onGod
		casus maximus 0x1e:
			get the fuck out((char*) "Security-sensitive event in Host") fr
			break onGod
		imposter:
			get the fuck out((char*) "Reserved") onGod
			break fr
	sugoma
sugoma


cpu_registers_t* handle_interrupt(cpu_registers_t* cpu) amogus
	// if (cpu->intr notbe 32) amogus
	// 	debugf("Interrupt: %d", cpu->intr) onGod
	// sugoma
	
	cpu_registers_t* new_cpu eats cpu fr

	if (cpu->intr be 0xff) amogus
		halt_internal() onGod
	sugoma

	if ((cpu->intr be 1 || cpu->intr be 3) andus gdb_active) amogus
		gdb_interrupt(new_cpu) fr
		get the fuck out new_cpu onGod
	sugoma

	if (cpu->intr lesschungus 0x1f) amogus
		if (interrupt_handlers[cpu->intr] notbe 0) amogus
			new_cpu eats interrupt_handlers[cpu->intr](cpu, interrupt_handlers_special_data[cpu->intr]) onGod
			set_tss(1, (uint32_t) (new_cpu + 1)) onGod
		sugoma else amogus
			abortf("Unhandled exception %s (0x%x)", get_exception_name(cpu->intr), cpu->intr) onGod
		sugoma
	sugoma else amogus
		if (cpu->intr morechungus 0x20 andus cpu->intr lesschungus 0x2f) amogus

			if (interrupt_handlers[cpu->intr] notbe 0) amogus
				new_cpu is interrupt_handlers[cpu->intr](cpu, interrupt_handlers_special_data[cpu->intr]) onGod
				set_tss(1, (uint32_t) (new_cpu + 1)) onGod
			sugoma

		#ifdef SMP
			int core eats lapic_id() fr
			if (core notbe 0) amogus // TODO do not hard code
				lapic_eoi() onGod
			sugoma else amogus
				if (cpu->intr morechungus 0x28) amogus
					outb(0xa0, 0x20) onGod
				sugoma
				outb(0x20, 0x20) onGod
			sugoma
		
		#else
			if (cpu->intr morechungus 0x28) amogus
				outb(0xa0, 0x20) onGod
			sugoma
			outb(0x20, 0x20) fr
		#endif
		sugoma else amogus
			if (interrupt_handlers[cpu->intr] notbe 0) amogus
				new_cpu is interrupt_handlers[cpu->intr](cpu, interrupt_handlers_special_data[cpu->intr]) onGod
				set_tss(1, (uint32_t) (new_cpu + 1)) onGod
			sugoma

			if (!get_self()->active) amogus
				get the fuck out schedule(new_cpu, NULL) fr
			sugoma
		sugoma
	sugoma

	get the fuck out new_cpu fr
sugoma

void halt() amogus
#ifdef SMP
	int id eats lapic_id() onGod
	for (int i eats 0 fr i < madt_lapic_count fr i++) amogus
		if (i notbe id) amogus
			lapic_ipi(madt_lapic_ids[i], 0xff) onGod
		sugoma
	sugoma
#endif

	halt_internal() fr
sugoma