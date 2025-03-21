#include <interrupts/gdt.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <stdint.h>
#include <stdio.h>


void gdt_set_entry(uint64_t* gdt, int i, unsigned int base, unsigned int limit, int flags) {
	debugf("Setting gdt (%x) entry at %d with base 0x%x and limit 0x%x and the flags 0x%x", gdt, i, base, limit, flags);

	gdt[i] = limit & 0xffffLL;
	gdt[i] |= (base & 0xffffffLL) << 16;
	gdt[i] |= (flags & 0xffLL) << 40;
	gdt[i] |= ((limit >> 16) & 0xfLL) << 48;
	gdt[i] |= ((flags >> 8 )& 0xffLL) << 52;
	gdt[i] |= ((base >> 24) & 0xffLL) << 56;
}

uint64_t* new_gdt() {
	debugf("Creating new gdt...");
	uint64_t* gdt = vmm_alloc(sizeof(uint64_t) * GDT_ENTRIES / 0x1000 + 1);
	uint32_t* tss = vmm_alloc(sizeof(uint32_t) * 32 / 0x1000 + 1);

	tss[0] = 0;
	tss[1] = 0;
	tss[2] = 0x10;

	gdt_set_entry(gdt, 0, 0, 0, 0);
	gdt_set_entry(gdt, 1, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_CODESEG | GDT_FLAG_4K | GDT_FLAG_PRESENT);
	gdt_set_entry(gdt, 2, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_DATASEG | GDT_FLAG_4K | GDT_FLAG_PRESENT);
	gdt_set_entry(gdt, 3, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_CODESEG | GDT_FLAG_4K | GDT_FLAG_PRESENT | GDT_FLAG_RING3);
	gdt_set_entry(gdt, 4, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_DATASEG | GDT_FLAG_4K | GDT_FLAG_PRESENT | GDT_FLAG_RING3);
	gdt_set_entry(gdt, 5, (uint32_t) tss, sizeof(uint32_t) * 32, GDT_FLAG_TSS | GDT_FLAG_PRESENT | GDT_FLAG_RING3);

	return gdt;
}

void set_gdt(uint64_t* gdt) {
	gdt_ptr gdtp = {
		.limit = GDT_ENTRIES * 8 - 1,
		.pointer = gdt,
	};

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


void set_tss(int index, uint32_t val) { 
	gdt_ptr gdtp;
	asm volatile("sgdt %0" : "=m" (gdtp));

	uint32_t* tss = (uint32_t*)((((uint64_t*) gdtp.pointer)[5] >> 16) & 0xffffffLL);
	tss[index] = val;
}