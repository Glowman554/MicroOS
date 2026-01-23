#pragma once

#include <driver/nic_driver.h>
#include <driver/pci/pci.h>
#include <stdbool.h>
#include <stdint.h>
#include <interrupts/interrupts.h>

typedef struct initialization_block {
	uint16_t mode;
	unsigned reserved1 : 4;
	unsigned num_send_buffers : 4;
	unsigned reserved2 : 4;
	unsigned num_recv_buffers : 4;
	uint64_t physical_address : 48;
	uint16_t reserved3;
	uint64_t logical_address;
	uint32_t recv_buffer_descr_address;
	uint32_t send_buffer_descr_address;
} __attribute__((packed)) initialization_block_t;

typedef struct buffer_descriptor {
	uint32_t address;
	uint32_t flags;
	uint32_t flags2;
	uint32_t avail;
} __attribute__((packed)) buffer_descriptor_t;

typedef struct am79C973_driver {
	nic_driver_t driver;
	pci_device_t header;
	uint16_t base_port;

	buffer_descriptor_t* send_buffer_descr;
	uint8_t* send_buffer_descr_memory;
	uint8_t* send_buffers;
	uint8_t current_send_buffer;
			
	buffer_descriptor_t* recv_buffer_descr;
	uint8_t* recv_buffer_descr_memory;
	uint8_t* recv_buffers;
	uint8_t current_recv_buffer;

	initialization_block_t* init_block;
} am79C973_driver_t;

nic_driver_t* get_am79C973_driver(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function);

bool am79C973_is_device_present(driver_t* driver);
char* am79C973_get_device_name(driver_t* driver);
void am79C973_init(driver_t* driver);

void am79C973_send(nic_driver_t* driver, async_t* async, uint8_t* data, uint32_t size);
void am79C973_stack(nic_driver_t* driver, void* stack);

cpu_registers_t* am79C973_interrupt(cpu_registers_t* regs, void* data);
void am79C973_recieve(am79C973_driver_t* driver);
mac_u am79C973_get_mac(am79C973_driver_t* driver);

void am79C973_pci_found(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function);