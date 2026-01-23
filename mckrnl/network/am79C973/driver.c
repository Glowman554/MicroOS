#include <am79C973.h>
#include <string.h>
#include <memory/vmm.h>
#include <utils/io.h>
#include <stdio.h>
#include <driver/pci/pci_bar.h>

nic_driver_t* get_am79C973_driver(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) {
	am79C973_driver_t* driver = vmm_alloc(PAGES_OF(am79C973_driver_t));
	memset(driver, 0, sizeof(am79C973_driver_t));

	*driver = (am79C973_driver_t) {
		.driver = {
			.driver = {
				.get_device_name = am79C973_get_device_name,
				.is_device_present = am79C973_is_device_present,
				.init = am79C973_init
			},
			.send = am79C973_send,
			.stack = am79C973_stack
		},
		.header = {
			.header = header,
			.bus = bus,
			.device = device,
			.function = function
		},
		.send_buffer_descr_memory = vmm_calloc(1),
		.send_buffers = vmm_calloc(5),
		.recv_buffer_descr_memory = vmm_calloc(1),
		.recv_buffers = vmm_calloc(5),
		.init_block = vmm_calloc(1)
	};

	return (nic_driver_t*) driver;
}

bool am79C973_is_device_present(driver_t* driver) {
	return true;
}

char* am79C973_get_device_name(driver_t* driver) {
	return "am79C973";
}

void am79C973_init(driver_t* driver) {
	am79C973_driver_t* am_driver = (am79C973_driver_t*) driver;

	enable_bus_master(am_driver->header.bus, am_driver->header.device, am_driver->header.function);

	am_driver->base_port = pci_get_io_port(&am_driver->header.header, am_driver->header.bus, am_driver->header.device, am_driver->header.function);
	debugf("am79C973: base_port: %d", am_driver->base_port);

	debugf("am79C973: interrupt %d", am_driver->header.header.interrupt_line + 0x20);
	register_interrupt_handler(am_driver->header.header.interrupt_line + 0x20, am79C973_interrupt, am_driver);

	inw(am_driver->base_port + 0x14);
	outw(am_driver->base_port + 0x14, 0);

	am_driver->driver.mac = am79C973_get_mac(am_driver);
	debugf("am79C973: mac: %x:%x:%x:%x:%x:%x", am_driver->driver.mac.mac_p[0], am_driver->driver.mac.mac_p[1], am_driver->driver.mac.mac_p[2], am_driver->driver.mac.mac_p[3], am_driver->driver.mac.mac_p[4], am_driver->driver.mac.mac_p[5]);

	am_driver->init_block->mode = 0x0000; // promiscuous mode = false
	am_driver->init_block->reserved1 = 0;
	am_driver->init_block->num_send_buffers = 3;
	am_driver->init_block->reserved2 = 0;
	am_driver->init_block->num_recv_buffers = 3;
	am_driver->init_block->physical_address = am_driver->driver.mac.mac;
	am_driver->init_block->reserved3 = 0;
	am_driver->init_block->logical_address = 0;

	uint16_t register_data_port = am_driver->base_port + 0x10;
	uint16_t register_address_port = am_driver->base_port + 0x12;
	uint16_t bus_control_register_data_port = am_driver->base_port + 0x16;

	// enable 32 bit mode
	outw(register_address_port, 20);
	outw(bus_control_register_data_port, 0x102);

	// stop reset
	outw(register_address_port, 0x0);
	outw(register_data_port, 0x4);

	am_driver->send_buffer_descr = (buffer_descriptor_t*) ((((uint32_t) &am_driver->send_buffer_descr_memory[0]) + 15) & ~((uint32_t) 0xF));
	am_driver->init_block->send_buffer_descr_address = (uint32_t) am_driver->send_buffer_descr;
	am_driver->recv_buffer_descr = (buffer_descriptor_t*) ((((uint32_t) &am_driver->recv_buffer_descr_memory[0]) + 15) & ~((uint32_t) 0xF));
	am_driver->init_block->recv_buffer_descr_address = (uint32_t) am_driver->recv_buffer_descr;

	for (int i = 0; i < 8; i++) {
		am_driver->send_buffer_descr[i].address = (((uint32_t) &am_driver->send_buffers[i]) + 15) & ~(uint32_t) 0xF;
		am_driver->send_buffer_descr[i].flags = 0x7FF | 0xF000;
		am_driver->send_buffer_descr[i].flags2 = 0;
		am_driver->send_buffer_descr[i].avail = 0;
		
		am_driver->recv_buffer_descr[i].address = (((uint32_t) &am_driver->recv_buffers[i]) + 15) & ~(uint32_t) 0xF;
		am_driver->recv_buffer_descr[i].flags = 0xF7FF | 0x80000000;
		am_driver->recv_buffer_descr[i].flags2 = 0;
		am_driver->recv_buffer_descr[i].avail = 0;
	}

	outw(register_address_port, 0x1);
	outw(register_data_port, (uint32_t) am_driver->init_block & 0xffff);
	outw(register_address_port, 0x2);
	outw(register_data_port, ((uint32_t) am_driver->init_block >> 16) & 0xffff);

	outw(register_address_port, 0x0);
	outw(register_data_port, 0x41);

	outw(register_address_port, 0x4);
	uint32_t tmp = inw(register_data_port);
	outw(register_address_port, 0x4);
	outw(register_data_port, tmp | 0xc00);

	outw(register_address_port, 0x0);
	outw(register_data_port, 0x42);

	register_nic_driver((nic_driver_t*) am_driver);
}

void am79C973_send(nic_driver_t* driver, async_t* async, uint8_t* data, uint32_t size) {
	am79C973_driver_t* am_driver = (am79C973_driver_t*) driver;

	switch (async->state) {
		case STATE_INIT:
			{
				am_driver->init_block->logical_address = driver->ip_config.ip.ip;

				uint16_t register_data_port = am_driver->base_port + 0x10;
				uint16_t register_address_port = am_driver->base_port + 0x12;

				int send_descriptor = am_driver->current_send_buffer;
				am_driver->current_send_buffer = (am_driver->current_send_buffer + 1) % 8;

				if (size > 1518) {
					debugf("am79C973: packet too long");
					size = 1518;
				}

				memcpy((uint8_t*) am_driver->send_buffer_descr[send_descriptor].address, data, size);
				
				am_driver->send_buffer_descr[send_descriptor].avail = 0;
				am_driver->send_buffer_descr[send_descriptor].flags2 = 0;
				am_driver->send_buffer_descr[send_descriptor].flags = 0x8300F000 | ((uint16_t)((-size) & 0xFFF));

				outw(register_address_port, 0x0);
				outw(register_data_port, 0x48);
				async->state = STATE_DONE;
			}
			break;
	}
}

void am79C973_stack(nic_driver_t* driver, void* stack) {}


cpu_registers_t* am79C973_interrupt(cpu_registers_t* regs, void* data) {
	am79C973_driver_t* am_driver = (am79C973_driver_t*) data;

	uint16_t register_data_port = am_driver->base_port + 0x10;
	uint16_t register_address_port = am_driver->base_port + 0x12;

	outw(register_address_port, 0x0);
	uint32_t temp = inw(register_data_port);

	debugf("am79C973: interrupt: %x", temp);

	if ((temp & 0x100) == 0x100) {
		debugf("am79C973: init done");
	}

	if((temp & 0x8000) == 0x8000) {
		debugf("am79c973: ERROR");
	}

	if((temp & 0x2000) == 0x2000) {
		debugf("am79c973: COLLISION ERROR");
	}

	if((temp & 0x1000) == 0x1000) {
		debugf("am79c973: MISSED FRAME");
	}

	if((temp & 0x0800) == 0x0800) {
		debugf("am79c973: MEMORY ERROR");
	}

	if((temp & 0x0400) == 0x0400) {
		debugf("am79c973: RECEIVE");
	}

	if((temp & 0x0200) == 0x0200) {
		debugf("am79c973: TRANSMIT");
	}

	if ((temp & 0x0400) == 0x0400) {
		am79C973_recieve(am_driver);
	}

	outw(register_address_port, 0x0);
	outw(register_data_port, temp);

	return regs;
}

void am79C973_recieve(am79C973_driver_t* driver) {
	for (; (driver->recv_buffer_descr[driver->current_recv_buffer].flags & 0x80000000) == 0; driver->current_recv_buffer = (driver->current_recv_buffer + 1) % 8) {
		if (!(driver->recv_buffer_descr[driver->current_recv_buffer].flags & 0x40000000) && (driver->recv_buffer_descr[driver->current_recv_buffer].flags & 0x03000000) == 0x03000000) {
			uint32_t size = driver->recv_buffer_descr[driver->current_recv_buffer].flags & 0xFFF;

			debugf("am79C973: received packet of size %d", size);

			if (size > 64) {
				size -= 4;
			}

			uint8_t* data = (uint8_t*) driver->recv_buffer_descr[driver->current_recv_buffer].address;
			driver->driver.recv((nic_driver_t*) driver, data, size);

			driver->recv_buffer_descr[driver->current_recv_buffer].flags2 = 0;
			driver->recv_buffer_descr[driver->current_recv_buffer].flags = 0x8000F7FF;

		} else {
			debugf("am79C973: packet not ready");
		}
	}
}

mac_u am79C973_get_mac(am79C973_driver_t* driver) {
	uint8_t mac0 = inw(driver->base_port + 0x00) % 256;
	uint8_t mac1 = inw(driver->base_port + 0x00) / 256;
	uint8_t mac2 = inw(driver->base_port + 0x02) % 256;
	uint8_t mac3 = inw(driver->base_port + 0x02) / 256;
	uint8_t mac4 = inw(driver->base_port + 0x04) % 256;
	uint8_t mac5 = inw(driver->base_port + 0x04) / 256;

	return (mac_u) {
		.mac_p = {
			mac0,
			mac1,
			mac2,
			mac3,
			mac4,
			mac5
		}
	};
}

void am79C973_pci_found(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) {
	register_driver((driver_t*) get_am79C973_driver(header, bus, device, function));
}