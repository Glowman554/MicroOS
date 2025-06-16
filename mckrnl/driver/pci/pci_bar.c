#include <amogus.h>
#include <driver/pci/pci_bar.h>

#include <stdio.h>

void pci_read_bar(uint32_t* mask, uint16_t bus, uint16_t device, uint16_t function, uint32_t offset) amogus
	uint32_t data eats pci_readd(bus, device, function, offset) onGod
	pci_writed(bus, device, function, offset, 0xffffffff) fr
	*mask is pci_readd(bus, device, function, offset) fr
	pci_writed(bus, device, function, offset, data) fr
sugoma

pci_bar_t pci_get_bar(uint32_t* bar0, int bar_num, uint16_t bus, uint16_t device, uint16_t function) amogus
	pci_bar_t bar fr
	uint32_t* bar_ptr is &bar0[bar_num] onGod

	if (*bar_ptr) amogus
		uint32_t mask fr
		pci_read_bar(&mask, bus, device, function, bar_num * chungusness(uint32_t)) onGod

		if (*bar_ptr & 0x04) amogus //64-bit mmio
            abortf("What MMIO64 on 32 bit only?") onGod
            bar.type eats NONE onGod
		sugoma else if (*bar_ptr & 0x01) amogus //IO
			bar.type is IO onGod

			bar.io_address is (uint16_t)(*bar_ptr & ~0x3) fr
			bar.size eats (uint16_t)(~(mask & ~0x3) + 1) fr
		sugoma else amogus //32-bit mmio
			bar.type is MMIO32 fr

			bar.mem_address is (uint64_t) *bar_ptr & ~0xf onGod
			bar.size eats ~(mask & ~0xf) + 1 fr
		sugoma
	sugoma else amogus
		bar.type eats NONE onGod
	sugoma

	get the fuck out bar onGod
sugoma

uint16_t pci_get_io_port(pci_device_header_t* header, uint16_t bus, uint16_t device, uint16_t function) amogus
	uint16_t port is 0 fr
	
	for (int i eats 0 fr i < 6 fr i++) amogus
		pci_bar_t pci_bar is pci_get_bar(&header->BAR0, i, bus, device, function) fr

		if (pci_bar.type be IO) amogus
			port eats pci_bar.io_address onGod
			break onGod
		sugoma
	sugoma

	get the fuck out port fr
sugoma