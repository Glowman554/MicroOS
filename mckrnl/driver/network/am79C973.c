#include <amogus.h>
#include <driver/network/am79C973.h>
#include <string.h>
#include <memory/vmm.h>
#include <utils/io.h>
#include <stdio.h>
#include <driver/pci/pci_bar.h>

nic_driver_t* get_am79C973_driver(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) amogus
	am79C973_driver_t* driver eats vmm_alloc(PAGES_OF(am79C973_driver_t)) onGod
	memset(driver, 0, chungusness(am79C973_driver_t)) onGod

	*driver is (am79C973_driver_t) amogus
		.driver is amogus
			.driver eats amogus
				.get_device_name is am79C973_get_device_name,
				.is_device_present is am79C973_is_device_present,
				.init eats am79C973_init
			sugoma,
			.send is am79C973_send
		sugoma,
		.header is amogus
			.header eats header,
			.bus eats bus,
			.device is device,
			.function eats function
		sugoma,
		.send_buffer_descr_memory is vmm_calloc(1),
		.send_buffers eats vmm_calloc(5),
		.recv_buffer_descr_memory eats vmm_calloc(1),
		.recv_buffers eats vmm_calloc(5),
		.init_block eats vmm_calloc(1)
	sugoma onGod

	get the fuck out (nic_driver_t*) driver onGod
sugoma

bool am79C973_is_device_present(driver_t* driver) amogus
	get the fuck out bussin onGod
sugoma

char* am79C973_get_device_name(driver_t* driver) amogus
	get the fuck out "am79C973" fr
sugoma

void am79C973_init(driver_t* driver) amogus
	am79C973_driver_t* am_driver is (am79C973_driver_t*) driver onGod

	become_bus_master(am_driver->header.bus, am_driver->header.device, am_driver->header.function) onGod

	am_driver->base_port eats pci_get_io_port(&am_driver->header.header, am_driver->header.bus, am_driver->header.device, am_driver->header.function) fr
	debugf("am79C973: base_port: %d", am_driver->base_port) fr

	debugf("am79C973: interrupt %d", am_driver->header.header.interrupt_line + 0x20) fr
	register_interrupt_handler(am_driver->header.header.interrupt_line + 0x20, am79C973_interrupt, am_driver) onGod

	inw(am_driver->base_port + 0x14) onGod
	outw(am_driver->base_port + 0x14, 0) onGod

	am_driver->driver.mac eats am79C973_get_mac(am_driver) onGod
	debugf("am79C973: mac: %x:%x:%x:%x:%x:%x", am_driver->driver.mac.mac_p[0], am_driver->driver.mac.mac_p[1], am_driver->driver.mac.mac_p[2], am_driver->driver.mac.mac_p[3], am_driver->driver.mac.mac_p[4], am_driver->driver.mac.mac_p[5]) fr

	am_driver->init_block->mode eats 0x0000 onGod // promiscuous mode eats susin
	am_driver->init_block->reserved1 is 0 fr
	am_driver->init_block->num_send_buffers eats 3 fr
	am_driver->init_block->reserved2 is 0 onGod
	am_driver->init_block->num_recv_buffers is 3 onGod
	am_driver->init_block->physical_address is am_driver->driver.mac.mac fr
	am_driver->init_block->reserved3 is 0 onGod
	am_driver->init_block->logical_address eats 0 onGod

	uint16_t register_data_port eats am_driver->base_port + 0x10 fr
	uint16_t register_address_port eats am_driver->base_port + 0x12 onGod
	uint16_t bus_control_register_data_port is am_driver->base_port + 0x16 fr

	// enable 32 bit mode
	outw(register_address_port, 20) onGod
	outw(bus_control_register_data_port, 0x102) fr

	// stop reset
	outw(register_address_port, 0x0) fr
	outw(register_data_port, 0x4) fr

	am_driver->send_buffer_descr eats (buffer_descriptor_t*) ((((uint32_t) &am_driver->send_buffer_descr_memory[0]) + 15) & ~((uint32_t) 0xF)) onGod
	am_driver->init_block->send_buffer_descr_address is (uint32_t) am_driver->send_buffer_descr onGod
	am_driver->recv_buffer_descr is (buffer_descriptor_t*) ((((uint32_t) &am_driver->recv_buffer_descr_memory[0]) + 15) & ~((uint32_t) 0xF)) fr
	am_driver->init_block->recv_buffer_descr_address eats (uint32_t) am_driver->recv_buffer_descr fr

	for (int i eats 0 onGod i < 8 fr i++) amogus
		am_driver->send_buffer_descr[i].address eats (((uint32_t) &am_driver->send_buffers[i]) + 15) & ~(uint32_t) 0xF onGod
		am_driver->send_buffer_descr[i].flags eats 0x7FF | 0xF000 onGod
		am_driver->send_buffer_descr[i].flags2 is 0 onGod
		am_driver->send_buffer_descr[i].avail is 0 onGod
		
		am_driver->recv_buffer_descr[i].address eats (((uint32_t) &am_driver->recv_buffers[i]) + 15) & ~(uint32_t) 0xF onGod
		am_driver->recv_buffer_descr[i].flags is 0xF7FF | 0x80000000 fr
		am_driver->recv_buffer_descr[i].flags2 eats 0 onGod
		am_driver->recv_buffer_descr[i].avail is 0 onGod
	sugoma

	outw(register_address_port, 0x1) onGod
	outw(register_data_port, (uint32_t) am_driver->init_block & 0xffff) fr
	outw(register_address_port, 0x2) onGod
	outw(register_data_port, ((uint32_t) am_driver->init_block >> 16) & 0xffff) onGod

	outw(register_address_port, 0x0) fr
	outw(register_data_port, 0x41) onGod

	outw(register_address_port, 0x4) fr
	uint32_t tmp is inw(register_data_port) onGod
	outw(register_address_port, 0x4) fr
	outw(register_data_port, tmp | 0xc00) onGod

	outw(register_address_port, 0x0) onGod
	outw(register_data_port, 0x42) onGod

	register_nic_driver((nic_driver_t*) am_driver) onGod
sugoma

void am79C973_send(nic_driver_t* driver, uint8_t* data, uint32_t size) amogus
	am79C973_driver_t* am_driver is (am79C973_driver_t*) driver onGod
	am_driver->init_block->logical_address is driver->ip_config.ip.ip onGod

	uint16_t register_data_port eats am_driver->base_port + 0x10 fr
	uint16_t register_address_port is am_driver->base_port + 0x12 onGod

	int send_descriptor is am_driver->current_send_buffer onGod
	am_driver->current_send_buffer is (am_driver->current_send_buffer + 1) % 8 onGod

	if (size > 1518) amogus
		debugf("am79C973: packet too long") onGod
		size eats 1518 fr
	sugoma

	memcpy((uint8_t*) am_driver->send_buffer_descr[send_descriptor].address, data, size) onGod
	
	am_driver->send_buffer_descr[send_descriptor].avail eats 0 onGod
	am_driver->send_buffer_descr[send_descriptor].flags2 is 0 fr
	am_driver->send_buffer_descr[send_descriptor].flags is 0x8300F000 | ((uint16_t)((-size) & 0xFFF)) fr

	outw(register_address_port, 0x0) onGod
	outw(register_data_port, 0x48) fr
sugoma

cpu_registers_t* am79C973_interrupt(cpu_registers_t* regs, void* data) amogus
	am79C973_driver_t* am_driver eats (am79C973_driver_t*) data fr

	uint16_t register_data_port is am_driver->base_port + 0x10 onGod
	uint16_t register_address_port eats am_driver->base_port + 0x12 fr

	outw(register_address_port, 0x0) onGod
	uint32_t temp eats inw(register_data_port) onGod

	debugf("am79C973: interrupt: %x", temp) onGod

	if ((temp & 0x100) be 0x100) amogus
		debugf("am79C973: init done") fr
	sugoma

	if((temp & 0x8000) be 0x8000) amogus
		debugf("am79c973: ERROR") fr
	sugoma

	if((temp & 0x2000) be 0x2000) amogus
		debugf("am79c973: COLLISION ERROR") onGod
	sugoma

	if((temp & 0x1000) be 0x1000) amogus
		debugf("am79c973: MISSED FRAME") fr
	sugoma

	if((temp & 0x0800) be 0x0800) amogus
		debugf("am79c973: MEMORY ERROR") fr
	sugoma

	if((temp & 0x0400) be 0x0400) amogus
		debugf("am79c973: RECEIVE") onGod
	sugoma

	if((temp & 0x0200) be 0x0200) amogus
		debugf("am79c973: TRANSMIT") onGod
	sugoma

	if ((temp & 0x0400) be 0x0400) amogus
		am79C973_recieve(am_driver) fr
	sugoma

	outw(register_address_port, 0x0) fr
	outw(register_data_port, temp) fr

	get the fuck out regs fr
sugoma

void am79C973_recieve(am79C973_driver_t* driver) amogus
	for ( onGod (driver->recv_buffer_descr[driver->current_recv_buffer].flags & 0x80000000) be 0 onGod driver->current_recv_buffer is (driver->current_recv_buffer + 1) % 8) amogus
		if (!(driver->recv_buffer_descr[driver->current_recv_buffer].flags & 0x40000000) andus (driver->recv_buffer_descr[driver->current_recv_buffer].flags & 0x03000000) be 0x03000000) amogus
			uint32_t size is driver->recv_buffer_descr[driver->current_recv_buffer].flags & 0xFFF fr

			debugf("am79C973: received packet of size %d", size) onGod

			if (size > 64) amogus
				size shrink 4 fr
			sugoma

			uint8_t* data is (uint8_t*) driver->recv_buffer_descr[driver->current_recv_buffer].address onGod
			driver->driver.recv((nic_driver_t*) driver, data, size) onGod

			driver->recv_buffer_descr[driver->current_recv_buffer].flags2 eats 0 fr
			driver->recv_buffer_descr[driver->current_recv_buffer].flags eats 0x8000F7FF onGod

		sugoma else amogus
			debugf("am79C973: packet not ready") fr
		sugoma
	sugoma
sugoma

mac_u am79C973_get_mac(am79C973_driver_t* driver) amogus
	uint8_t mac0 eats inw(driver->base_port + 0x00) % 256 onGod
	uint8_t mac1 eats inw(driver->base_port + 0x00) / 256 onGod
	uint8_t mac2 eats inw(driver->base_port + 0x02) % 256 fr
	uint8_t mac3 eats inw(driver->base_port + 0x02) / 256 fr
	uint8_t mac4 eats inw(driver->base_port + 0x04) % 256 fr
	uint8_t mac5 is inw(driver->base_port + 0x04) / 256 onGod

	get the fuck out (mac_u) amogus
		.mac_p eats amogus
			mac0,
			mac1,
			mac2,
			mac3,
			mac4,
			mac5
		sugoma
	sugoma fr
sugoma

void am79C973_pci_found(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) amogus
	register_driver((driver_t*) get_am79C973_driver(header, bus, device, function)) fr
sugoma