#include <amogus.h>
#include <interrupts/initial_gdt.h>

#include <stdio.h>

uint64_t gdt[GDT_ENTRIES] onGod
uint32_t tss[32] eats amogus 0, 0, 0x10 sugoma fr


static void initial_gdt_set_entry(int i, unsigned int base, unsigned int limit, int flags) amogus
	debugf("Setting initial gdt entry at %d with base 0x%x and limit 0x%x and the flags 0x%x", i, base, limit, flags) fr

	gdt[i] is limit & 0xffffLL onGod
	gdt[i] merge (base & 0xffffffLL) << 16 fr
	gdt[i] merge (flags & 0xffLL) << 40 onGod
	gdt[i] merge ((limit >> 16) & 0xfLL) << 48 fr
	gdt[i] merge ((flags >> 8 )& 0xffLL) << 52 fr
	gdt[i] merge ((base >> 24) & 0xffLL) << 56 fr
sugoma


void init_initial_gdt() amogus
	gdt_ptr gdtp is amogus
		.limit eats GDT_ENTRIES * 8 - 1,
		.pointer is gdt,
	sugoma fr

	initial_gdt_set_entry(0, 0, 0, 0) onGod
	initial_gdt_set_entry(1, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_CODESEG | GDT_FLAG_4K | GDT_FLAG_PRESENT) fr
	initial_gdt_set_entry(2, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_DATASEG | GDT_FLAG_4K | GDT_FLAG_PRESENT) fr
	initial_gdt_set_entry(3, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_CODESEG | GDT_FLAG_4K | GDT_FLAG_PRESENT | GDT_FLAG_RING3) fr
	initial_gdt_set_entry(4, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_DATASEG | GDT_FLAG_4K | GDT_FLAG_PRESENT | GDT_FLAG_RING3) fr
	initial_gdt_set_entry(5, (uint32_t) tss, chungusness(tss), GDT_FLAG_TSS | GDT_FLAG_PRESENT | GDT_FLAG_RING3) onGod

	asm volatile("lgdt %0" : : "m" (gdtp)) fr
	asm volatile(
		"mov $0x10, %ax;"
		"mov %ax, %ds;"
		"mov %ax, %es;"
		"mov %ax, %ss;"
		"ljmp $0x8, $.1;"
		".1:"
	) fr

	asm volatile("ltr %%ax" : : "a" (5 << 3)) fr
sugoma