#include <driver/pci/pci_bar.h>

#include <stdio.h>

void pci_read_bar(uint32_t* mask, uint16_t bus, uint16_t device, uint16_t function, uint32_t offset) {
	uint32_t data = pci_readd(bus, device, function, offset);
	pci_writed(bus, device, function, offset, 0xffffffff);
	*mask = pci_readd(bus, device, function, offset);
	pci_writed(bus, device, function, offset, data);
}

pci_bar_t pci_get_bar(uint32_t* bar0, int bar_num, uint16_t bus, uint16_t device, uint16_t function) {
	pci_bar_t bar;
	uint32_t* bar_ptr = &bar0[bar_num];

	if (*bar_ptr) {
		uint32_t mask;
		pci_read_bar(&mask, bus, device, function, bar_num * sizeof(uint32_t));

		if (*bar_ptr & 0x04) { //64-bit mmio
            abortf("What MMIO64 on 32 bit only?");
            bar.type = NONE;
		} else if (*bar_ptr & 0x01) { //IO
			bar.type = IO;

			bar.io_address = (uint16_t)(*bar_ptr & ~0x3);
			bar.size = (uint16_t)(~(mask & ~0x3) + 1);
		} else { //32-bit mmio
			bar.type = MMIO32;

			bar.mem_address = (uint64_t) *bar_ptr & ~0xf;
			bar.size = ~(mask & ~0xf) + 1;
		}
	} else {
		bar.type = NONE;
	}

	return bar;
}

uint16_t pci_get_io_port(pci_device_header_t* header, uint16_t bus, uint16_t device, uint16_t function) {
	uint16_t port = 0;
	
	for (int i = 0; i < 6; i++) {
		pci_bar_t pci_bar = pci_get_bar(&header->BAR0, i, bus, device, function);

		if (pci_bar.type == IO) {
			port = pci_bar.io_address;
			break;
		}
	}

	return port;
}