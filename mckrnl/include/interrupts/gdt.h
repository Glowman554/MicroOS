#pragma once

#include <interrupts/initial_gdt.h>

uint64_t* new_gdt();
void set_gdt(uint64_t* gdt);
void set_tss(int index, uint32_t val);