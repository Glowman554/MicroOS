#include <interrupts/initial_gdt.h>

#include <stdio.h>

uint64_t gdt[GDT_ENTRIES];
uint32_t tss[32] = { 0, 0, 0x10 };


static void initial_gdt_set_entry(int i, unsigned int base, unsigned int limit, int flags) {
	debugf("Setting initial gdt entry at %d with base 0x%x and limit 0x%x and the flags 0x%x", i, base, limit, flags);

	gdt[i] = limit & 0xffffLL;
	gdt[i] |= (base & 0xffffffLL) << 16;
	gdt[i] |= (flags & 0xffLL) << 40;
	gdt[i] |= ((limit >> 16) & 0xfLL) << 48;
	gdt[i] |= ((flags >> 8 )& 0xffLL) << 52;
	gdt[i] |= ((base >> 24) & 0xffLL) << 56;
}


void init_initial_gdt() {
	gdt_ptr gdtp = {
		.limit = GDT_ENTRIES * 8 - 1,
		.pointer = gdt,
	};

	initial_gdt_set_entry(0, 0, 0, 0);
	initial_gdt_set_entry(1, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_CODESEG | GDT_FLAG_4K | GDT_FLAG_PRESENT);
	initial_gdt_set_entry(2, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_DATASEG | GDT_FLAG_4K | GDT_FLAG_PRESENT);
	initial_gdt_set_entry(3, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_CODESEG | GDT_FLAG_4K | GDT_FLAG_PRESENT | GDT_FLAG_RING3);
	initial_gdt_set_entry(4, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_DATASEG | GDT_FLAG_4K | GDT_FLAG_PRESENT | GDT_FLAG_RING3);
	initial_gdt_set_entry(5, (uint32_t) tss, sizeof(tss), GDT_FLAG_TSS | GDT_FLAG_PRESENT | GDT_FLAG_RING3);

	asm volatile("lgdt %0" : : "m" (gdtp));
	asm volatile(
		"mov $0x10, %ax;"
		"mov %ax, %ds;"
		"mov %ax, %es;"
		"mov %ax, %ss;"
		"ljmp $0x8, $.1;"
		".1:"
	);

	asm volatile("ltr %%ax" : : "a" (5 << 3));
}