#include <driver/apic/lapic.h>
#include <driver/acpi/madt.h>

void lapic_write(uint32_t reg, uint32_t value) {
	*((volatile uint32_t*)(madt_lapic_base_addr + reg)) = value;
}

uint32_t lapic_read(uint32_t reg) {
	return *((volatile uint32_t*)(madt_lapic_base_addr + reg));
}

void lapic_wait() {
	do {
		__asm__ __volatile__ ("pause" : : : "memory");
	} while(*((volatile uint32_t*)(madt_lapic_base_addr + 0x300)) & (1 << 12));
}

void lapic_eoi() {
	lapic_write(0xB0, 0);
}

void lapic_enable() {
	lapic_write(0xF0, lapic_read(0xF0) | 0x1ff);
}

void lapic_ipi(uint8_t lapic_id, uint8_t vector) {
	lapic_write(0x310, lapic_id << 24);
	lapic_write(0x300, vector);
}

#pragma GCC push_options
#pragma GCC optimize ("O0")
int lapic_id() {
	volatile unsigned char core_id = 0; 	
	__asm__ __volatile__ ("mov $1, %%eax; cpuid; shrl $24, %%ebx;": "=b"(core_id) : : );

	return core_id;
}
#pragma GCC pop_options
