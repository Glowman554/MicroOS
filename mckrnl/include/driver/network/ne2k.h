#pragma once

#include <driver/nic_driver.h>
#include <driver/pci/pci.h>
#include <stdbool.h>
#include <stdint.h>
#include <interrupts/interrupts.h>

#define NE_CMD 0
#define NE_PSTART 1
#define NE_PSTOP 2
#define NE_BNDRY 3

#define NE_CMD 0
#define NE_PSTART 1
#define NE_PSTOP 2
#define NE_BNDRY 3

#define NE_TSR 4 // R
#define NE_TPSR 4 // W

#define NE_TBCR0 5
#define NE_TBCR1 6

#define NE_ISR 7

#define NE_RSAR0 8
#define NE_RSAR1 9
#define NE_RBCR0 10
#define NE_RBCR1 11

#define NE_RCR 12
#define NE_TCR 13
#define NE_DCR 14
#define NE_IMR 15

#define NE_PAR 1
#define NE_CURR 7
#define NE_MAR 8

#define PAGE_TX 0x40
#define PAGE_RX 0x50
#define PAGE_STOP 0x80

#define NE_RESET 0x1F
#define NE_DATA 0x10


typedef struct ne2k_driver {
	nic_driver_t driver;
	pci_device_t header;
	uint16_t io_base;
    uint8_t next_packet;
    bool is_transmitting;
} ne2k_driver_t;


nic_driver_t* get_ne2k_driver(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function);

bool ne2k_is_device_present(driver_t* driver);
char* ne2k_get_device_name(driver_t* driver);
void ne2k_init(driver_t* driver);

void ne2k_send(nic_driver_t* driver, async_t* async, uint8_t* data, uint32_t size);
void ne2k_stack(nic_driver_t* driver, void* stack);

cpu_registers_t* ne2k_interrupt(cpu_registers_t* regs, void* data);
void ne2k_receive(ne2k_driver_t* driver);
mac_u ne2k_get_mac(ne2k_driver_t* driver);

void ne2k_pci_found(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function);