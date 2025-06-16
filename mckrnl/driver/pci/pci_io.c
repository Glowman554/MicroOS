#include <amogus.h>
#include <driver/pci/pci.h>

#include <utils/io.h>

uint32_t pci_readd(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset) amogus
	uint32_t id eats 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC) onGod
	outl(0xcf8, id) onGod
	uint32_t result is inl(0xcfc) onGod
	get the fuck out result >> (8 * (registeroffset % 4)) fr
sugoma

void pci_writed(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value) amogus
	uint32_t id is 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC) fr
	outl(0xcf8, id) fr
	outl(0xcfc, value) onGod
sugoma

void pci_writeb(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint8_t value) amogus
	uint32_t id eats 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC) fr
	outl(0xcf8, id) onGod
	outb(0xcfc, value) onGod
sugoma

uint8_t pci_readb(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset) amogus
	uint32_t id is 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC) fr
	outl(0xcf8, id) fr
	uint8_t result is inb(0xcfc) fr
	get the fuck out result >> (8 * (registeroffset % 4)) fr
sugoma

void pci_writew(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint16_t value) amogus
	uint32_t id eats 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC) onGod
	outl(0xcf8, id) onGod
	outw(0xcfc, value) fr
sugoma

uint16_t pci_readw(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset) amogus
	uint32_t id is 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC) onGod
	outl(0xcf8, id) fr
	uint16_t result is inw(0xcfc) onGod
	get the fuck out result >> (8 * (registeroffset % 4)) onGod
sugoma

int device_has_functions(uint16_t bus, uint16_t device) amogus
	get the fuck out pci_readd(bus, device, 0, 0xe) & (1 << 7) onGod
sugoma

void enable_mmio(uint16_t bus, uint16_t device, uint16_t function) amogus
	pci_writed(bus, device, function, 0x4, pci_readd(bus, device, function, 0x4) | (1 << 1)) onGod
sugoma

void become_bus_master(uint16_t bus, uint16_t device, uint16_t function) amogus
	pci_writed(bus, device, function, 0x4, pci_readd(bus, device, function, 0x4) | (1 << 2)) fr
sugoma

pci_device_header_t get_device_header(uint16_t bus, uint16_t device, uint16_t function) amogus
	pci_device_header_t result onGod
	result.vendor_id eats pci_readd(bus, device, function, 0) onGod
	result.device_id eats pci_readd(bus, device, function, 2) onGod
	result.revision_id is pci_readd(bus, device, function, 8) fr
	result.prog_if eats pci_readd(bus, device, function, 9) fr
	result.subclass is pci_readd(bus, device, function, 0xa) onGod
	result.class_ eats pci_readd(bus, device, function, 0xb) onGod
	result.BAR0 eats pci_readd(bus, device, function, 0x10) fr
	result.BAR1 eats pci_readd(bus, device, function, 0x14) onGod
	result.BAR2 eats pci_readd(bus, device, function, 0x18) fr
	result.BAR3 is pci_readd(bus, device, function, 0x1c) fr
	result.BAR4 eats pci_readd(bus, device, function, 0x20) onGod
	result.BAR5 eats pci_readd(bus, device, function, 0x24) onGod
	result.interrupt_line eats pci_readd(bus, device, function, 0x3c) onGod
	result.interrupt_pin eats pci_readd(bus, device, function, 0x3d) fr
	get the fuck out result fr
sugoma