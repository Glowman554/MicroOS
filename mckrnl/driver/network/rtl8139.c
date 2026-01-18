#include "driver/pci/pci.h"
#include <driver/network/rtl8139.h>
#include <string.h>
#include <memory/vmm.h>
#include <utils/io.h>
#include <stdio.h>

uint8_t TSAD_array[4] = { 0x20, 0x24, 0x28, 0x2C };
uint8_t TSD_array[4] = { 0x10, 0x14, 0x18, 0x1C };

nic_driver_t* get_rtl8139_driver(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) {
	rtl8139_driver_t* driver = vmm_alloc(PAGES_OF(rtl8139_driver_t));
	memset(driver, 0, sizeof(rtl8139_driver_t));

	*driver = (rtl8139_driver_t) {
		.driver = {
			.driver = {
				.get_device_name = rtl8139_get_device_name,
				.is_device_present = rtl8139_is_device_present,
				.init = rtl8139_init
			},
			.send = rtl8139_send,
			.stack = rtl8139_stack
		},
		.header = {
			.header = header,
			.bus = bus,
			.device = device,
			.function = function
		},
		.rx_buffer = vmm_alloc((RX_BUF_SIZE + 16 + 1500) / 0x1000 + 1)
	};

	return (nic_driver_t*) driver;
}

bool rtl8139_is_device_present(driver_t* driver) {
	return true;
}

char* rtl8139_get_device_name(driver_t* driver) {
	return "rtl8139";
}

void rtl8139_init(driver_t* driver) {
	rtl8139_driver_t* rtl_driver = (rtl8139_driver_t*) driver;

	rtl_driver->bar_type = (rtl_driver->header.header.BAR0 & 1);
	rtl_driver->io_base = (rtl_driver->header.header.BAR0 & (~3));
	rtl_driver->mem_base = (rtl_driver->header.header.BAR0 & (~15));

	debugf("rtl8139: bar_type: %d, io_base: %x, mem_base: %x", rtl_driver->bar_type, rtl_driver->io_base, rtl_driver->mem_base);

	enable_bus_master(rtl_driver->header.bus, rtl_driver->header.device, rtl_driver->header.function);

	
	outb(rtl_driver->io_base + 0x52, 0x00);

	outb(rtl_driver->io_base + 0x37, 0x10);
	while((inb(rtl_driver->io_base + 0x37) & 0x10) != 0) {
		__asm__ __volatile__("pause" ::: "memory");
	}

	outl(rtl_driver->io_base + 0x30, (uint32_t) rtl_driver->rx_buffer);

	// Sets the TOK and ROK bits high
	outw(rtl_driver->io_base + 0x3C, 0x0005);

	// (1 << 7) is the WRAP bit, 0xf is AB+AM+APM+AAP
	outl(rtl_driver->io_base + 0x44, 0xf | (1 << 7));

	// Sets the RE and TE bits high
	outb(rtl_driver->io_base + 0x37, 0x0C);

	rtl_driver->driver.mac = rtl8139_get_mac(rtl_driver);
	debugf("rtl8139: mac: %x:%x:%x:%x:%x:%x", rtl_driver->driver.mac.mac_p[0], rtl_driver->driver.mac.mac_p[1], rtl_driver->driver.mac.mac_p[2], rtl_driver->driver.mac.mac_p[3], rtl_driver->driver.mac.mac_p[4], rtl_driver->driver.mac.mac_p[5]);

	register_interrupt_handler(rtl_driver->header.header.interrupt_line + 0x20, rtl8139_interrupt, rtl_driver);
	register_nic_driver((nic_driver_t*) rtl_driver);
}

void rtl8139_send(nic_driver_t* driver, async_t* async, uint8_t* data, uint32_t size) {
	rtl8139_driver_t* rtl_driver = (rtl8139_driver_t*) driver;

	switch (async->state) {
		case STATE_INIT:
			outl(rtl_driver->io_base + TSAD_array[rtl_driver->tx_cur], (uint32_t) data);
			outl(rtl_driver->io_base + TSD_array[rtl_driver->tx_cur++], size);

			if(rtl_driver->tx_cur > 3) {
				rtl_driver->tx_cur = 0;
			}
			async->state = STATE_DONE;
			break;	
	}
}

void rtl8139_stack(nic_driver_t* driver, void* stack) {}


cpu_registers_t* rtl8139_interrupt(cpu_registers_t* regs, void* data) {
	rtl8139_driver_t* rtl_driver = (rtl8139_driver_t*) data;

	uint16_t status = inw(rtl_driver->io_base + 0x3e);
	outw(rtl_driver->io_base + 0x3E, 0x5);

	if(status & TOK) {
		debugf("rtl8139: Packet sent");
	}

	if (status & ROK) {
		rtl8139_recieve(rtl_driver);
	}

	return regs;
}

void rtl8139_recieve(rtl8139_driver_t* driver) {
	uint16_t * t = (uint16_t*)(driver->rx_buffer + driver->current_packet_ptr);
	uint16_t packet_length = *(t + 1);

	debugf("rtl8139: Received packet of length %d", packet_length);

	t = t + 2;

	driver->driver.recv((nic_driver_t*) driver, (uint8_t*) t, packet_length - 4);

	driver->current_packet_ptr = (driver->current_packet_ptr + packet_length + 4 + 3) & RX_READ_POINTER_MASK;

	if(driver->current_packet_ptr > RX_BUF_SIZE) {
		driver->current_packet_ptr -= RX_BUF_SIZE;
	}

	outw(driver->io_base + CAPR, driver->current_packet_ptr - 0x10);
}

mac_u rtl8139_get_mac(rtl8139_driver_t* driver) {
	uint32_t mac_part1 = inl(driver->io_base + 0x00);
	uint16_t mac_part2 = inw(driver->io_base + 0x04);

	return (mac_u) {
		.mac_p = {
			mac_part1 >> 0,
			mac_part1 >> 8,
			mac_part1 >> 16,
			mac_part1 >> 24,

			mac_part2 >> 0,
			mac_part2 >> 8
		}
	};
}

void rtl8139_pci_found(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) {
	register_driver((driver_t*) get_rtl8139_driver(header, bus, device, function));
}