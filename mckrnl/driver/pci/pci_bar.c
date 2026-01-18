#include <driver/pci/pci_bar.h>

#include <stdio.h>
#include <utils/io.h>

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
            abortf(false, "What MMIO64 on 32 bit only?");
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



uint8_t pci_bar_read_byte(pci_bar_t type, uint8_t field) {
	if (type.type == MMIO32 || type.type == MMIO64) {
		return *(uint8_t*)(type.mem_address + field);
	} else if (type.type == IO) {
		return inb(type.io_address + field);
	}
	return 0;
}

uint16_t pci_bar_read_word(pci_bar_t type, uint8_t field) {
	if (type.type == MMIO32 || type.type == MMIO64) {
		return *(uint16_t*)(type.mem_address + field);
	} else if (type.type == IO) {
		return inw(type.io_address + field);
	}
	return 0;
}

uint32_t pci_bar_read_dword(pci_bar_t type, uint8_t field) {
	if (type.type == MMIO32 || type.type == MMIO64) {
		return *(uint32_t*)(type.mem_address + field);
	} else if (type.type == IO) {
		return inl(type.io_address + field);
	}
	return 0;
}

void pci_bar_write_byte(pci_bar_t type, uint16_t field, uint8_t value) {
	if (type.type == MMIO32 || type.type == MMIO64) {
		*(uint8_t*)(type.mem_address + field) = value;
	} else if (type.type == IO) {
		outb(type.io_address + field, value);
	}
}

void pci_bar_write_word(pci_bar_t type, uint16_t field, uint16_t value) {
	if (type.type == MMIO32 || type.type == MMIO64) {
		*(uint16_t*)(type.mem_address + field) = value;
	} else if (type.type == IO) {
		outw(type.io_address + field, value);
	}
}

void pci_bar_write_dword(pci_bar_t type, uint16_t field, uint32_t value) {
	if (type.type == MMIO32 || type.type == MMIO64) {
		*(uint32_t*)(type.mem_address + field) = value;
	} else if (type.type == IO) {
		outl(type.io_address + field, value);
	}
}