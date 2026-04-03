#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <driver/pci/pci.h>
#include <driver/usb.h>
#include <interrupts/interrupts.h>

#define UHCI_REG_USBCMD     0x00 
#define UHCI_REG_USBSTS     0x02 
#define UHCI_REG_USBINTR    0x04 
#define UHCI_REG_FRNUM      0x06 
#define UHCI_REG_FRBASEADD  0x08 
#define UHCI_REG_SOFMOD     0x0C 
#define UHCI_REG_PORTSC1    0x10 
#define UHCI_REG_PORTSC2    0x12 

#define UHCI_CMD_RS          (1 << 0)
#define UHCI_CMD_HCRESET     (1 << 1)   
#define UHCI_CMD_GRESET      (1 << 2)   
#define UHCI_CMD_EGSM        (1 << 3)   
#define UHCI_CMD_FGR         (1 << 4)   
#define UHCI_CMD_SWDBG       (1 << 5)   
#define UHCI_CMD_CF          (1 << 6)   
#define UHCI_CMD_MAXP        (1 << 7)   

#define UHCI_STS_USBINT      (1 << 0)
#define UHCI_STS_ERROR        (1 << 1)   
#define UHCI_STS_RD           (1 << 2)   
#define UHCI_STS_HSE          (1 << 3)   
#define UHCI_STS_HCPE         (1 << 4)   
#define UHCI_STS_HCH          (1 << 5)   

#define UHCI_INTR_TIMEOUT     (1 << 0)   
#define UHCI_INTR_RESUME      (1 << 1)   
#define UHCI_INTR_IOC         (1 << 2)   
#define UHCI_INTR_SP          (1 << 3)   

#define UHCI_PORT_CONNECTION      (1 << 0)
#define UHCI_PORT_CONNECTION_CHG  (1 << 1)   
#define UHCI_PORT_ENABLE          (1 << 2)   
#define UHCI_PORT_ENABLE_CHG      (1 << 3)   
#define UHCI_PORT_LINE_STATUS     (3 << 4)   
#define UHCI_PORT_RD              (1 << 6)   
#define UHCI_PORT_LSDA            (1 << 8)   
#define UHCI_PORT_RESET           (1 << 9)   
#define UHCI_PORT_SUSPEND         (1 << 12)  

#define UHCI_FRAME_LIST_SIZE      1024

#define UHCI_TD_LINK_TERMINATE    (1 << 0)
#define UHCI_TD_LINK_QH           (1 << 1)   
#define UHCI_TD_LINK_DEPTH        (1 << 2)   

#define UHCI_TD_STATUS_ACTIVE     (1 << 23)  
#define UHCI_TD_STATUS_STALLED    (1 << 22)  
#define UHCI_TD_STATUS_DATABUF    (1 << 21)  
#define UHCI_TD_STATUS_BABBLE     (1 << 20)  
#define UHCI_TD_STATUS_NAK        (1 << 19)  
#define UHCI_TD_STATUS_CRC        (1 << 18)  
#define UHCI_TD_STATUS_BITSTUFF   (1 << 17)  
#define UHCI_TD_STATUS_IOC        (1 << 24)  
#define UHCI_TD_STATUS_IOS        (1 << 25)  
#define UHCI_TD_STATUS_LS         (1 << 26)  
#define UHCI_TD_STATUS_SPD        (1 << 29)  

#define UHCI_TD_ERROR_MASK  (UHCI_TD_STATUS_STALLED | UHCI_TD_STATUS_DATABUF | \
                             UHCI_TD_STATUS_BABBLE | UHCI_TD_STATUS_CRC | \
                             UHCI_TD_STATUS_BITSTUFF)

typedef struct uhci_td {
	volatile uint32_t link;   
	volatile uint32_t status; 
	volatile uint32_t token;     
	volatile uint32_t buffer;    
	uint32_t sw_reserved[4];
} __attribute__((packed, aligned(16))) uhci_td_t;

typedef struct uhci_qh {
	volatile uint32_t head;      
	volatile uint32_t element;   
	uint32_t sw_reserved[2];
} __attribute__((packed, aligned(16))) uhci_qh_t;

#define UHCI_MAX_TDS  128
#define UHCI_MAX_QHS  16

typedef struct uhci_controller {
	pci_device_t pci_dev;
	uint16_t io_base;
	uint32_t* frame_list;

	uhci_qh_t* qh_pool;
	uhci_td_t* td_pool;
	int next_td;
	int next_qh;

	uhci_qh_t* qh_interrupt;

	usb_device_t devices[USB_MAX_DEVICES];
	uint8_t next_address;
	int num_ports;

	usb_hc_driver_t* hc_driver;
} uhci_controller_t;

void uhci_pci_found(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function);

bool uhci_is_device_present(driver_t* driver);
char* uhci_get_device_name(driver_t* driver);
void uhci_init(driver_t* driver);

cpu_registers_t* uhci_interrupt(cpu_registers_t* regs, void* data);

void uhci_reset(uhci_controller_t* hc);
void uhci_start(uhci_controller_t* hc);
void uhci_stop(uhci_controller_t* hc);

void uhci_port_reset(uhci_controller_t* hc, int port);
bool uhci_port_connected(uhci_controller_t* hc, int port);
uint8_t uhci_port_speed(uhci_controller_t* hc, int port);

uhci_td_t* uhci_alloc_td(uhci_controller_t* hc);
uhci_qh_t* uhci_alloc_qh(uhci_controller_t* hc);

void uhci_td_setup(uhci_td_t* td, uint8_t speed, uint8_t addr, uint8_t endp, uint8_t toggle, uint8_t pid, uint16_t len, void* data);
bool uhci_transfer_wait(uhci_controller_t* hc, uhci_td_t* td, int timeout_ms);
bool uhci_control_transfer_impl(uhci_controller_t* hc, usb_device_t* dev, usb_setup_packet_t* setup, void* data, uint16_t len);
bool uhci_interrupt_transfer_impl(uhci_controller_t* hc, usb_device_t* dev, usb_endpoint_descriptor_t* ep, void* data, uint16_t len);
bool uhci_bulk_transfer_impl(uhci_controller_t* hc, usb_device_t* dev, usb_endpoint_descriptor_t* ep, void* data, uint16_t len, uint8_t* toggle);

void uhci_probe_ports(uhci_controller_t* hc);
bool uhci_enumerate_device(uhci_controller_t* hc, int port);
