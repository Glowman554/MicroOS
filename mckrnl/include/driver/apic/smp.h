#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint32_t gdt;
} trampoline_data_t;

extern bool cpu_started[256];

void smp_startup_all();

extern int bsp_id;
