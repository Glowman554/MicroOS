#include <driver/apic/smp.h>
#include <driver/apic/lapic.h>
#include <driver/acpi/madt.h>
#include <interrupts/interrupts.h>
#include <interrupts/gdt.h>
#include <memory/vmm.h>
#include <string.h>
#include <stdio.h>
#include <config.h>
#include <interrupts/gdt.h>
#include <fs/vfs.h>
#include <assert.h>

#define SMP_TRAMPOLINE_PAGE 8
#define SMP_TRAMPOLINE_ADDR (SMP_TRAMPOLINE_PAGE * 0x1000)

extern uint8_t smp_trampoline[];
extern int smp_trampoline_size;

bool cpu_started[256] = { false };
int bsp_id = 0;

void smp_ap_main() {
// init gdt
	set_gdt(new_gdt());

	// init interrupts
	idt_ptr idtp = {
		.limit = IDT_ENTRIES * 8 - 1,
		.pointer = idt,
	};
	asm volatile("lidt %0" : : "m" (idtp));
	debugf("Enabling interrupts!");
	asm volatile("sti");

	// init paging
	vmm_activate_context(kernel_context);
	debugf("Activating paging NOW!");
	uint32_t cr0;
	asm volatile("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0" : : "r" (cr0));

	lapic_enable();

	// *(uint8_t*) 0x0 = 1;
	while (1) {
		// printf("uwu");
	}
}

typedef struct {
	uint32_t entry;
	uint32_t stack;
	bool done;
} __attribute__((packed)) ap_info_t;

void smp_startup_all() {
	bsp_id = lapic_id();
	cpu_started[bsp_id] = true;
	debugf("bspid: %d", bsp_id);

	vmm_map_page(kernel_context, madt_lapic_base_addr, madt_lapic_base_addr, PTE_PRESENT | PTE_WRITE);
	debugf("Mapped LAPIC memory at %p", (void*) madt_lapic_base_addr);

	vmm_map_page(kernel_context, SMP_TRAMPOLINE_ADDR, SMP_TRAMPOLINE_ADDR, PTE_PRESENT | PTE_WRITE);
	memcpy((void*) SMP_TRAMPOLINE_ADDR, smp_trampoline, 0x1000);

	ap_info_t* info = (ap_info_t*) (void*) (SMP_TRAMPOLINE_ADDR + 0x1000);

	lapic_enable();

	for (int i = 0; i < madt_lapic_count; i++) {
		if(madt_lapic_ids[i] == bsp_id) {
			debugf("Skiping BSP at %d", madt_lapic_ids[i]);
			continue;
		}

		debugf("spinning up CPU %d using trampoline at 0x%x...", i, SMP_TRAMPOLINE_ADDR);
		info->stack = (uint32_t) vmm_alloc(KERNEL_STACK_SIZE_PAGES) + KERNEL_STACK_SIZE_PAGES * 0x1000;
		debugf("allocated stack at 0x%x", info->stack);
		info->entry = (uint32_t) smp_ap_main;
		info->done = false;

		lapic_write(0x280, 0);
		lapic_write(0x310, (lapic_read(0x310) & 0x00ffffff) | (i << 24));
		lapic_write(0x300, (lapic_read(0x300) & 0xfff00000) | 0x00C500);

		lapic_wait();

		lapic_write(0x280, 0);
		lapic_write(0x310, (lapic_read(0x310) & 0x00ffffff) | (i << 24));
		lapic_write(0x300, (lapic_read(0x300) & 0xfff00000) | 0x008500);

		lapic_wait();

		for (int j = 0; j < 2; j++) {
			lapic_write(0x280, 0);
			lapic_write(0x310, (lapic_read(0x310) & 0x00ffffff) | (i << 24));
			lapic_write(0x300, (lapic_read(0x300) & 0xfff0f800) | 0x000600 | SMP_TRAMPOLINE_PAGE);

			lapic_wait();
		}

		while (!info->done) {
			__asm__ __volatile__("pause" ::: "memory");
		}

		debugf("CPU %d spinup complete!", i);
		// wait();
	}
}