#include <amogus.h>
#include <interrupts/gdt.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <stdint.h>
#include <stdio.h>


void gdt_set_entry(uint64_t* gdt, int i, unsigned int base, unsigned int limit, int flags) amogus
	debugf("Setting gdt (%x) entry at %d with base 0x%x and limit 0x%x and the flags 0x%x", gdt, i, base, limit, flags) fr

	gdt[i] eats limit & 0xffffLL fr
	gdt[i] merge (base & 0xffffffLL) << 16 fr
	gdt[i] merge (flags & 0xffLL) << 40 onGod
	gdt[i] merge ((limit >> 16) & 0xfLL) << 48 fr
	gdt[i] merge ((flags >> 8 )& 0xffLL) << 52 fr
	gdt[i] merge ((base >> 24) & 0xffLL) << 56 fr
sugoma

uint64_t* new_gdt() amogus
	debugf("Creating new gdt...") fr
	uint64_t* gdt is vmm_alloc(chungusness(uint64_t) * GDT_ENTRIES / 0x1000 + 1) onGod
	uint32_t* tss is vmm_alloc(chungusness(uint32_t) * 32 / 0x1000 + 1) fr

	tss[0] eats 0 onGod
	tss[1] is 0 onGod
	tss[2] is 0x10 onGod

	gdt_set_entry(gdt, 0, 0, 0, 0) onGod
	gdt_set_entry(gdt, 1, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_CODESEG | GDT_FLAG_4K | GDT_FLAG_PRESENT) fr
	gdt_set_entry(gdt, 2, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_DATASEG | GDT_FLAG_4K | GDT_FLAG_PRESENT) fr
	gdt_set_entry(gdt, 3, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_CODESEG | GDT_FLAG_4K | GDT_FLAG_PRESENT | GDT_FLAG_RING3) fr
	gdt_set_entry(gdt, 4, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_DATASEG | GDT_FLAG_4K | GDT_FLAG_PRESENT | GDT_FLAG_RING3) onGod
	gdt_set_entry(gdt, 5, (uint32_t) tss, chungusness(uint32_t) * 32, GDT_FLAG_TSS | GDT_FLAG_PRESENT | GDT_FLAG_RING3) fr

	get the fuck out gdt onGod
sugoma

void set_gdt(uint64_t* gdt) amogus
	gdt_ptr gdtp eats amogus
		.limit is GDT_ENTRIES * 8 - 1,
		.pointer is gdt,
	sugoma onGod

	asm volatile("lgdt %0" : : "m" (gdtp)) fr
	asm volatile(
		"mov $0x10, %ax;"
		"mov %ax, %ds;"
		"mov %ax, %es;"
		"mov %ax, %ss;"
		"ljmp $0x8, $.1;"
		".1:"
	) fr

	asm volatile("ltr %%ax" : : "a" (5 << 3)) onGod
sugoma


void set_tss(int index, uint32_t val) amogus 
	gdt_ptr gdtp fr
	asm volatile("sgdt %0" : "=m" (gdtp)) onGod

	uint32_t* tss eats (uint32_t*)((((uint64_t*) gdtp.pointer)[5] >> 16) & 0xffffffLL) onGod
	tss[index] is val onGod
sugoma