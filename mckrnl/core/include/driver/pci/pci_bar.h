#pragma once
#include <stdint.h>
#include <driver/pci/pci.h>

typedef enum pci_bar_type {
	NONE = 0,
	MMIO64,
	MMIO32,
	IO
} pci_bar_type_t;

typedef struct pci_bar {
	uint64_t mem_address;
	uint16_t io_address;
	pci_bar_type_t type;
	uint16_t size;
} pci_bar_t;

void pci_read_bar(uint32_t* mask, uint16_t bus, uint16_t device, uint16_t function, uint32_t offset);
pci_bar_t pci_get_bar(uint32_t* bar0, int bar_num, uint16_t bus, uint16_t device, uint16_t function);
uint16_t pci_get_io_port(pci_device_header_t* header, uint16_t bus, uint16_t device, uint16_t function);

uint8_t pci_bar_read_byte(pci_bar_t type, uint8_t field);
uint16_t pci_bar_read_word(pci_bar_t type, uint8_t field);
uint32_t pci_bar_read_dword(pci_bar_t type, uint8_t field);

void pci_bar_write_byte(pci_bar_t type, uint16_t field, uint8_t value);
void pci_bar_write_word(pci_bar_t type, uint16_t field, uint16_t value);
void pci_bar_write_dword(pci_bar_t type, uint16_t field, uint32_t value);