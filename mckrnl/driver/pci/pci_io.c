#include <driver/pci/pci.h>

#include <utils/io.h>

uint32_t pci_readd(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset) {
	uint32_t id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC);
	outl(0xcf8, id);
	uint32_t result = inl(0xcfc);
	return result >> (8 * (registeroffset % 4));
}

void pci_writed(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value) {
	uint32_t id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC);
	outl(0xcf8, id);
	outl(0xcfc, value);
}

void pci_writeb(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint8_t value) {
	uint32_t id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC);
	outl(0xcf8, id);
	outb(0xcfc, value);
}

uint8_t pci_readb(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset) {
	uint32_t id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC);
	outl(0xcf8, id);
	uint8_t result = inb(0xcfc);
	return result >> (8 * (registeroffset % 4));
}

void pci_writew(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint16_t value) {
	uint32_t id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC);
	outl(0xcf8, id);
	outw(0xcfc, value);
}

uint16_t pci_readw(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset) {
	uint32_t id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC);
	outl(0xcf8, id);
	uint16_t result = inw(0xcfc);
	return result >> (8 * (registeroffset % 4));
}

int device_has_functions(uint16_t bus, uint16_t device) {
	return pci_readd(bus, device, 0, 0xe) & (1 << 7);
}

void enable_mmio(uint16_t bus, uint16_t device, uint16_t function) {
	pci_writed(bus, device, function, 0x4, pci_readd(bus, device, function, 0x4) | (1 << 1));
}

void become_bus_master(uint16_t bus, uint16_t device, uint16_t function) {
	pci_writed(bus, device, function, 0x4, pci_readd(bus, device, function, 0x4) | (1 << 2));
}

pci_device_header_t get_device_header(uint16_t bus, uint16_t device, uint16_t function) {
	pci_device_header_t result;
	result.vendor_id = pci_readd(bus, device, function, 0);
	result.device_id = pci_readd(bus, device, function, 2);
	result.revision_id = pci_readd(bus, device, function, 8);
	result.prog_if = pci_readd(bus, device, function, 9);
	result.subclass = pci_readd(bus, device, function, 0xa);
	result.class_ = pci_readd(bus, device, function, 0xb);
	result.BAR0 = pci_readd(bus, device, function, 0x10);
	result.BAR1 = pci_readd(bus, device, function, 0x14);
	result.BAR2 = pci_readd(bus, device, function, 0x18);
	result.BAR3 = pci_readd(bus, device, function, 0x1c);
	result.BAR4 = pci_readd(bus, device, function, 0x20);
	result.BAR5 = pci_readd(bus, device, function, 0x24);
	result.interrupt_line = pci_readd(bus, device, function, 0x3c);
	result.interrupt_pin = pci_readd(bus, device, function, 0x3d);
	return result;
}