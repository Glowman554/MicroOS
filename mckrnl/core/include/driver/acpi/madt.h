#pragma once

#include <stdint.h>

extern uint8_t* madt_lapic_ids;
extern uint8_t madt_lapic_count;
extern uint32_t madt_lapic_base_addr;

extern uint8_t* madt_ioapic_ids;
extern uint32_t* madt_ioapic_base_addr;
extern uint8_t madt_ioapic_count;

void parse_madt();