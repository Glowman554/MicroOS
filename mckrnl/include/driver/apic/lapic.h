#pragma once

#include <stdint.h>

#define LAPIC_ID(vaiable_name) \
		volatile unsigned char vaiable_name = 0; \
		{\
			__asm__ __volatile__ ("mov $1, %%eax; cpuid; shrl $24, %%ebx;": "=b"(vaiable_name) : : ); \
		}

void lapic_write(uint32_t reg, uint32_t value);
uint32_t lapic_read(uint32_t reg);
void lapic_wait();
void lapic_eoi();
void lapic_enable();
void lapic_ipi(uint8_t lapic_id, uint8_t vector);