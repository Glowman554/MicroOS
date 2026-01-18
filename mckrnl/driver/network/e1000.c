#include "driver/pci/pci.h"
#include <driver/network/e1000.h>
#include <string.h>
#include <memory/vmm.h>
#include <utils/io.h>
#include <utils/mmio.h>
#include <stdio.h>
#include <driver/pci/pci_bar.h>

nic_driver_t* get_e1000_driver(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) {
	e1000_driver_t* driver = vmm_alloc(PAGES_OF(e1000_driver_t));
	memset(driver, 0, sizeof(e1000_driver_t));

	*driver = (e1000_driver_t) {
		.driver = {
			.driver = {
				.get_device_name = e1000_get_device_name,
				.is_device_present = e1000_is_device_present,
				.init = e1000_init
			},
			.send = e1000_send,
			.stack = e1000_stack
		},
		.header = {
			.header = header,
			.bus = bus,
			.device = device,
			.function = function
		},
	};

	return (nic_driver_t*) driver;
}

bool e1000_is_device_present(driver_t* driver) {
	return true;
}

char* e1000_get_device_name(driver_t* driver) {
	return "e1000";
}

void e1000_init(driver_t* driver) {
	e1000_driver_t* e1000_driver = (e1000_driver_t*) driver;
	enable_mmio(e1000_driver->header.bus, e1000_driver->header.device, e1000_driver->header.function);

	debugf("e1000: interrupt %d", e1000_driver->header.header.interrupt_line + 0x20);
	register_interrupt_handler(e1000_driver->header.header.interrupt_line + 0x20, e1000_interrupt, driver);

	for (int i = 0; i < 6; i++) {
		pci_bar_t pci_bar = pci_get_bar(&e1000_driver->header.header.BAR0, i, e1000_driver->header.bus, e1000_driver->header.device, e1000_driver->header.function);

		if (pci_bar.type == MMIO32 || pci_bar.type == MMIO64) {
			e1000_driver->bar_type = 0;
			e1000_driver->mem_base = pci_bar.mem_address;
			for (int i = 0; i < pci_bar.size / 0x1000 + 1; i++) {
				vmm_map_page(kernel_context, e1000_driver->mem_base + 0x1000 * i, e1000_driver->mem_base + 0x1000 * i, PTE_PRESENT | PTE_WRITE);
			}
			debugf("e1000: MMIO address: 0x%x", e1000_driver->mem_base);
			debugf("e1000: SIZE = %d; BAR TYPE = %d", pci_bar.size, pci_bar.type);
			break;
		} else if (pci_bar.type == IO) {
			e1000_driver->bar_type = 1;
			e1000_driver->io_port = pci_bar.io_address;
			debugf("e1000: IO port: %d", e1000_driver->io_port);
			debugf("e1000: SIZE = %d; BAR TYPE = %d", pci_bar.size, pci_bar.type);
			break;
		}
	}

	e1000_driver->rx_ptr = (uint8_t*) vmm_alloc(((sizeof(struct e1000_rx_desc) * e1000_NUM_RX_DESC + 16) / 0x1000) + 1);
	e1000_driver->tx_ptr = (uint8_t*) vmm_alloc(((sizeof(struct e1000_tx_desc) * e1000_NUM_TX_DESC + 16) / 0x1000) + 1);

	enable_bus_master(e1000_driver->header.bus, e1000_driver->header.device, e1000_driver->header.function);

	if (e1000_driver->bar_type == 0) {
		if (!(e1000_driver->mem_base)) {
			abortf(false, "e1000: an error occurred when getting the device's MMIO base address.");
			return;
		}
	} else {
		if (!(e1000_driver->io_port)) {
			abortf(false, "e1000: an error occurred when getting the device's IO port.");
			return;
		}
	}

	e1000_detect_eeprom(e1000_driver);

	e1000_driver->driver.mac = e1000_get_mac(e1000_driver);
	debugf("e1000: mac: %x:%x:%x:%x:%x:%x", e1000_driver->driver.mac.mac_p[0], e1000_driver->driver.mac.mac_p[1], e1000_driver->driver.mac.mac_p[2], e1000_driver->driver.mac.mac_p[3], e1000_driver->driver.mac.mac_p[4], e1000_driver->driver.mac.mac_p[5]);

	e1000_start_link(e1000_driver);
	for(int i = 0; i < 0x80; i++) {
		e1000_write_command(e1000_driver, 0x5200 + i * 4, 0);
	}

	//Enable interrupts
	e1000_write_command(e1000_driver, REG_IMASK, 0x1F6DC);
	e1000_write_command(e1000_driver, REG_IMASK, 0xff & ~4);
	e1000_read_command(e1000_driver, 0xc0);

	//Enable receiving and transmitting
	e1000_rx_init(e1000_driver);
	e1000_tx_init(e1000_driver);

	debugf("e1000: successfully activated!");

	register_nic_driver((nic_driver_t*) e1000_driver);
}

void e1000_send(nic_driver_t* driver, async_t* async, uint8_t* data, uint32_t size) {
	e1000_driver_t* e1000_driver = (e1000_driver_t*) driver;
	send_data_t* send_data = (send_data_t*) async->data;

	switch (async->state) {
		case STATE_INIT:
			e1000_driver->tx_descs[e1000_driver->tx_cur]->addr = (uint32_t) data;
			e1000_driver->tx_descs[e1000_driver->tx_cur]->length = size;
			e1000_driver->tx_descs[e1000_driver->tx_cur]->cmd = CMD_EOP | CMD_IFCS | CMD_RS;
			e1000_driver->tx_descs[e1000_driver->tx_cur]->status = 0;

			send_data->driver_specific = e1000_driver->tx_cur;
			e1000_driver->tx_cur = (e1000_driver->tx_cur + 1) % e1000_NUM_TX_DESC;
			e1000_write_command(e1000_driver, REG_TXDESCTAIL, e1000_driver->tx_cur);
			async->state = STATE_WAIT;
			break;
			
		case STATE_WAIT:
			if (e1000_driver->tx_descs[send_data->driver_specific]->status & 0xff) {
				async->state = STATE_DONE;
			}
			break;	
	}
}

void e1000_stack(nic_driver_t* driver, void* stack) {}


cpu_registers_t* e1000_interrupt(cpu_registers_t* regs, void* data) {
	e1000_driver_t* driver = (e1000_driver_t*) data;
	e1000_write_command(driver, REG_IMASK, 0x1);

	uint32_t status = e1000_read_command(driver, 0xC0);
	if (status & 0x04) {
		e1000_start_link(driver);
	} else if(status & 0x10) {
		debugf("e1000: Good threshold");
	} else if(status & 0x80) {
		e1000_receive(driver);
	}

	return regs;
}

void e1000_receive(e1000_driver_t* driver) {
	while(driver->rx_descs[driver->rx_cur]->status & 0x1) {
		uint8_t* data = (uint8_t*) (uint32_t) driver->rx_descs[driver->rx_cur]->addr;
		uint16_t size = driver->rx_descs[driver->rx_cur]->length;

		debugf("e1000: Received packet of length %d", size);
		driver->driver.recv((nic_driver_t*) driver, data, size);

		driver->rx_descs[driver->rx_cur]->status = 0;
		uint16_t old_cur = driver->rx_cur;
		driver->rx_cur = (driver->rx_cur + 1) % e1000_NUM_RX_DESC;
		e1000_write_command(driver, REG_RXDESCTAIL, old_cur);
	}
}

mac_u e1000_get_mac(e1000_driver_t* driver) {
	mac_u mac = { 0 };
	if (driver->eerprom_exists) {
		uint32_t temp;
		temp = e1000_read_eeprom(driver, 0);
		mac.mac_p[0] = temp & 0xff;
		mac.mac_p[1] = temp >> 8;
		temp = e1000_read_eeprom(driver, 1);
		mac.mac_p[2] = temp & 0xff;
		mac.mac_p[3] = temp >> 8;
		temp = e1000_read_eeprom(driver, 2);
		mac.mac_p[4] = temp & 0xff;
		mac.mac_p[5] = temp >> 8;
	} else {
		uint32_t mac_low = e1000_read_command(driver, 0x5400);
		uint32_t mac_high = e1000_read_command(driver, 0x5404);

		mac.mac_p[0] = mac_low & 0xff;
        mac.mac_p[1] = mac_low >> 8 & 0xff;
        mac.mac_p[2] = mac_low >> 16 & 0xff;
        mac.mac_p[3] = mac_low >> 24 & 0xff;

        mac.mac_p[4] = mac_high & 0xff;
        mac.mac_p[5] = mac_high >> 8 & 0xff;
	}

	return mac;
}

void e1000_pci_found(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) {
	register_driver((driver_t*) get_e1000_driver(header, bus, device, function));
}

void e1000_write_command(e1000_driver_t* driver, uint16_t address, uint32_t value) {
	if (driver->bar_type == 0) {
		mmio_write32(driver->mem_base + address, value);
	} else {
		outl(driver->io_port, address);
		outl(driver->io_port + 4, value);
	}
}

uint32_t e1000_read_command(e1000_driver_t* driver, uint16_t address) {
	if (driver->bar_type == 0) {
		return mmio_read32(driver->mem_base + address);
	} else {
		outl(driver->io_port, address);
		return inl(driver->io_port + 4);
	}
}

bool e1000_detect_eeprom(e1000_driver_t* driver) {
	e1000_write_command(driver, REG_EEPROM, 0x1);

	for(int i = 0; i < 1000 && !driver->eerprom_exists; i++) {
		if(e1000_read_command(driver, REG_EEPROM) & 0x10) {
			driver->eerprom_exists = true;
		}
	}
	return driver->eerprom_exists;
}

uint32_t e1000_read_eeprom(e1000_driver_t* driver, uint8_t addr) {
	uint16_t data = 0;
	uint32_t tmp = 0;
	if (driver->eerprom_exists) {
		e1000_write_command(driver, REG_EEPROM, (1) | ((uint32_t)(addr) << 8));
		while(!((tmp = e1000_read_command(driver, REG_EEPROM)) & (1 << 4)));
	} else {
		e1000_write_command(driver, REG_EEPROM, (1) | ((uint32_t)(addr) << 2));
		while(!((tmp = e1000_read_command(driver, REG_EEPROM)) & (1 << 1)));
	}
	data = (uint16_t)((tmp >> 16) & 0xFFFF);
	return data;
}

void e1000_rx_init(e1000_driver_t* driver) {
	struct e1000_rx_desc* descs = (struct e1000_rx_desc*) driver->rx_ptr;
	for (int i = 0; i < e1000_NUM_RX_DESC; i++) {
		driver->rx_descs[i] = (struct e1000_rx_desc*) ((uint8_t*)descs + i * 16);
		driver->rx_descs[i]->addr = (uint32_t) vmm_alloc(((8192 + 16) / 0x1000) + 1);
		driver->rx_descs[i]->status = 0;
	}

	e1000_write_command(driver, REG_RXDESCLO, (uint32_t) driver->rx_ptr);
	e1000_write_command(driver, REG_RXDESCHI, 0);

	e1000_write_command(driver, REG_RXDESCLEN, e1000_NUM_RX_DESC * 16);

	e1000_write_command(driver, REG_RXDESCHEAD, 0);
	e1000_write_command(driver, REG_RXDESCTAIL, e1000_NUM_RX_DESC - 1);
	driver->rx_cur = 0;

	e1000_write_command(driver, REG_RCTRL, RCTL_EN| RCTL_SBP| RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC  | RCTL_BSIZE_8192);
}

void e1000_tx_init(e1000_driver_t* driver) {
	struct e1000_tx_desc* descs = (struct e1000_tx_desc*) driver->tx_ptr;
	for (int i = 0; i < e1000_NUM_TX_DESC; i++) {
		driver->tx_descs[i] = (struct e1000_tx_desc*) ((uint8_t*) descs + i * 16);
		driver->tx_descs[i]->addr = 0;
		driver->tx_descs[i]->cmd = 0;
		driver->tx_descs[i]->status = TSTA_DD;
	}

	e1000_write_command(driver, REG_TXDESCLO, (uint32_t) driver->tx_ptr);
	e1000_write_command(driver, REG_TXDESCHI, 0);

	e1000_write_command(driver, REG_TXDESCLEN, e1000_NUM_TX_DESC * 16);

	e1000_write_command(driver, REG_TXDESCHEAD, 0);
	e1000_write_command(driver, REG_TXDESCTAIL, 0);
	driver->tx_cur = 0;

	e1000_write_command(driver, REG_TCTRL, 0b0110000000000111111000011111010);
	e1000_write_command(driver, REG_TIPG, 0x0060200A);
}

void e1000_start_link(e1000_driver_t* driver) {
	uint32_t val = e1000_read_command(driver, REG_CTRL);
	e1000_write_command(driver, REG_CTRL, val | ECTRL_SLU);
}