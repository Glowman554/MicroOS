#pragma once
#include <stdint.h>

typedef struct {
	uint32_t gdt;
} trampoline_data_t;

void smp_startup_all();