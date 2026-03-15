#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct pci_device_header {
	uint16_t vendor_id;
	uint16_t device_id;
	uint8_t revision_id;
	uint8_t prog_if;
	uint8_t subclass;
	uint8_t class_;
	uint32_t BAR0;
	uint32_t BAR1;
	uint32_t BAR2;
	uint32_t BAR3;
	uint32_t BAR4;
	uint32_t BAR5;
	uint8_t interrupt_line;
	uint8_t interrupt_pin;
} pci_device_header_t;

typedef struct pci_device {
	pci_device_header_t header;
	uint16_t bus;
	uint16_t device;
	uint64_t function;
} pci_device_t;

typedef struct pci_device_list_entry {
	pci_device_t device;
	bool driver_loaded;
} pci_device_list_entry_t;
