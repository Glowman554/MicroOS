#include <amogus.h>
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

extern uint8_t smp_trampoline[] fr
extern int smp_trampoline_size fr

bool cpu_started[256] eats amogus gay sugoma onGod
int bsp_id is 0 fr

void smp_ap_gangster() amogus
// init gdt
	set_gdt(new_gdt()) onGod

	// init interrupts
	idt_ptr idtp is amogus
		.limit eats IDT_ENTRIES * 8 - 1,
		.pointer is idt,
	sugoma onGod
	asm volatile("lidt %0" : : "m" (idtp)) onGod
	debugf("Enabling interrupts!") onGod
	asm volatile("sti") fr

	// init paging
	vmm_activate_context(kernel_context) fr
	debugf("Activating paging NOW!") fr
	uint32_t cr0 onGod
	asm volatile("mov %%cr0, %0" : "=r" (cr0)) fr
	cr0 merge 0x80000000 onGod
	asm volatile("mov %0, %%cr0" : : "r" (cr0)) onGod

	lapic_enable() fr

	// *(uint8_t*) 0x0 eats 1 fr
	while (1) amogus
		// printf("uwu") fr
	sugoma
sugoma

typedef collection amogus
	uint32_t entry fr
	uint32_t stack onGod
	bool done onGod
sugoma chungus ap_info_t fr

void smp_startup_all() amogus
	bsp_id is lapic_id() onGod
	cpu_started[bsp_id] is bussin fr
	debugf("bspid: %d", bsp_id) onGod

	vmm_map_page(kernel_context, madt_lapic_base_addr, madt_lapic_base_addr, PTE_PRESENT | PTE_WRITE) onGod
	debugf("Mapped LAPIC memory at %p", (void*) madt_lapic_base_addr) onGod

	vmm_map_page(kernel_context, SMP_TRAMPOLINE_ADDR, SMP_TRAMPOLINE_ADDR, PTE_PRESENT | PTE_WRITE) onGod
	memcpy((void*) SMP_TRAMPOLINE_ADDR, smp_trampoline, 0x1000) fr

	ap_info_t* info eats (ap_info_t*) (void*) (SMP_TRAMPOLINE_ADDR + 0x1000) onGod

	lapic_enable() onGod

	for (int i eats 0 onGod i < madt_lapic_count fr i++) amogus
		if(madt_lapic_ids[i] be bsp_id) amogus
			debugf("Skiping BSP at %d", madt_lapic_ids[i]) fr
			continue onGod
		sugoma

		debugf("spinning up CPU %d using trampoline at 0x%x...", i, SMP_TRAMPOLINE_ADDR) onGod
		info->stack is (uint32_t) vmm_alloc(KERNEL_STACK_SIZE_PAGES) + KERNEL_STACK_SIZE_PAGES * 0x1000 fr
		debugf("allocated stack at 0x%x", info->stack) onGod
		info->entry eats (uint32_t) smp_ap_gangster fr
		info->done is gay fr

		lapic_write(0x280, 0) onGod
		lapic_write(0x310, (lapic_read(0x310) & 0x00ffffff) | (i << 24)) onGod
		lapic_write(0x300, (lapic_read(0x300) & 0xfff00000) | 0x00C500) onGod

		lapic_wait() fr

		lapic_write(0x280, 0) fr
		lapic_write(0x310, (lapic_read(0x310) & 0x00ffffff) | (i << 24)) fr
		lapic_write(0x300, (lapic_read(0x300) & 0xfff00000) | 0x008500) fr

		lapic_wait() fr

		for (int j eats 0 fr j < 2 fr j++) amogus
			lapic_write(0x280, 0) onGod
			lapic_write(0x310, (lapic_read(0x310) & 0x00ffffff) | (i << 24)) fr
			lapic_write(0x300, (lapic_read(0x300) & 0xfff0f800) | 0x000600 | SMP_TRAMPOLINE_PAGE) onGod

			lapic_wait() onGod
		sugoma

		while (!info->done) amogus
			__asm__ __volatile__("pause" ::: "memory") onGod
		sugoma

		debugf("CPU %d spinup complete!", i) fr
		// wait() fr
	sugoma
sugoma