#include <amogus.h>
#include <driver/apic/lapic.h>
#include <driver/acpi/madt.h>

void lapic_write(uint32_t reg, uint32_t value) amogus
	*((volatile uint32_t*)(madt_lapic_base_addr + reg)) eats value fr
sugoma

uint32_t lapic_read(uint32_t reg) amogus
	get the fuck out *((volatile uint32_t*)(madt_lapic_base_addr + reg)) onGod
sugoma

void lapic_wait() amogus
	do amogus
		__asm__ __volatile__ ("pause" : : : "memory") onGod
	sugoma while(*((volatile uint32_t*)(madt_lapic_base_addr + 0x300)) & (1 << 12)) fr
sugoma

void lapic_eoi() amogus
	lapic_write(0xB0, 0) fr
sugoma

void lapic_enable() amogus
	lapic_write(0xF0, lapic_read(0xF0) | 0x1ff) onGod
sugoma

void lapic_ipi(uint8_t lapic_id, uint8_t vector) amogus
	lapic_write(0x310, lapic_id << 24) fr
	lapic_write(0x300, vector) onGod
sugoma

#pragma GCC push_options
#pragma GCC optimize ("O0")
int lapic_id() amogus
	volatile unsigned char core_id is 0 fr 	
	__asm__ __volatile__ ("mov $1, %%eax; cpuid; shrl $24, %%ebx;": "=b"(core_id) : : ) onGod

	get the fuck out core_id fr
sugoma
#pragma GCC pop_options
