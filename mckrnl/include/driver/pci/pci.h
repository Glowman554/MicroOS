#pragma once

#include <stdint.h>
#include <stdbool.h>

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

typedef struct pci_driver {
	uint8_t _class;
	uint8_t subclass;
	uint8_t prog_IF;
	bool use_class_subclass_prog_IF;

	uint16_t vendor_id;
	uint16_t device_id;
	bool use_vendor_device_id;

	void (*load_driver)(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function);
} pci_driver_t;

typedef struct pci_device_list_entry {
	pci_device_t device;
	bool driver_loaded;
} pci_device_list_entry_t;

extern pci_device_list_entry_t* pci_devices;
extern int num_pci_devices;

const char* get_vendor_name(uint16_t vendor_ID);
const char* get_device_name(uint16_t vendor_ID, uint16_t device_ID);
const char* get_subclass_name(uint8_t class_code, uint8_t subclass_code);
const char* get_prog_IF_name(uint8_t class_code, uint8_t subclass_code, uint8_t prog_IF);

const char* get_device_class(uint8_t _class);

uint32_t pci_readd(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset);
void pci_writed(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value);

uint8_t pci_readb(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset);
void pci_writeb(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint8_t value);

uint16_t pci_readw(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset);
void pci_writew(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint16_t value);

int device_has_functions(uint16_t bus, uint16_t device);
pci_device_header_t get_device_header(uint16_t bus, uint16_t device, uint16_t function);

void enable_io(uint16_t bus, uint16_t device, uint16_t function);
void disable_io(uint16_t bus, uint16_t device, uint16_t function);

void enable_mmio(uint16_t bus, uint16_t device, uint16_t function);
void disable_mmio(uint16_t bus, uint16_t device, uint16_t function);

void enable_interrupt(uint16_t bus, uint16_t device, uint16_t function);
void disable_interrupt(uint16_t bus, uint16_t device, uint16_t function);

void enable_bus_master(uint16_t bus, uint16_t device, uint16_t function);
void disable_bus_master(uint16_t bus, uint16_t device, uint16_t function);

void enumerate_pci();

void register_pci_driver_cs(uint8_t _class, uint8_t subclass, uint8_t prog_IF, void (*load_driver)(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function));
void register_pci_driver_vd(uint16_t vendor_id, uint16_t device_id, void (*load_driver)(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function));
