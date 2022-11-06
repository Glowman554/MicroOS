#pragma once

#include <driver/nic_driver.h>
#include <driver/pci/pci.h>
#include <stdbool.h>
#include <stdint.h>
#include <interrupts/interrupts.h>

#define RX_BUF_SIZE 8192

#define CAPR 0x38
#define RX_READ_POINTER_MASK (~3)
#define ROK (1<<0)
#define RER (1<<1)
#define TOK (1<<2)
#define TER (1<<3)
#define TX_TOK (1<<15)

typedef struct rtl8139_driver {
	nic_driver_t driver;
	pci_device_t header;
	uint8_t bar_type;
	uint16_t io_base;
	uint32_t mem_base;
	uint8_t* rx_buffer;
	uint32_t tx_cur;
	uint32_t current_packet_ptr;
} rtl8139_driver_t;


nic_driver_t* get_rtl8139_driver(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function);

bool rtl8139_is_device_present(driver_t* driver);
char* rtl8139_get_device_name(driver_t* driver);
void rtl8139_init(driver_t* driver);

void rtl8139_send(nic_driver_t* driver, uint8_t* data, uint32_t size);

cpu_registers_t* rtl8139_interrupt(cpu_registers_t* regs, void* data);
void rtl8139_recieve(rtl8139_driver_t* driver);
mac_u rtl8139_get_mac(rtl8139_driver_t* driver);

void rtl8139_pci_found(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function);