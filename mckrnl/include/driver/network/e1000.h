#pragma once

#include <driver/nic_driver.h>
#include <driver/pci/pci.h>
#include <stdbool.h>
#include <stdint.h>
#include <interrupts/interrupts.h>

#define REG_CTRL		0x0000
#define REG_STATUS		0x0008
#define REG_EEPROM		0x0014
#define REG_CTRL_EXT	0x0018
#define REG_IMASK		0x00D0
#define REG_RCTRL		0x0100
#define REG_RXDESCLO	0x2800
#define REG_RXDESCHI	0x2804
#define REG_RXDESCLEN	0x2808
#define REG_RXDESCHEAD	0x2810
#define REG_RXDESCTAIL	0x2818

#define REG_TCTRL		0x0400
#define REG_TXDESCLO	0x3800
#define REG_TXDESCHI	0x3804
#define REG_TXDESCLEN	0x3808
#define REG_TXDESCHEAD	0x3810
#define REG_TXDESCTAIL	0x3818

#define REG_RDTR		0x2820 //RX Delay Timer Register
#define REG_RXDCTL		0x3828 //RX Descriptor Control
#define REG_RADV		0x282C //RX Int. Absolute Delay Timer
#define REG_RSRPD		0x2C00 //RX Small Packet Detect Interrupt

#define REG_TIPG		0x0410 //Transmit Inter Packet Gap
#define ECTRL_SLU		0x40 //Set link up

#define RCTL_EN							(1 << 1) //Receiver Enable
#define RCTL_SBP						(1 << 2) //Store Bad Packets
#define RCTL_UPE						(1 << 3) //Unicast Promiscuous Enabled
#define RCTL_MPE						(1 << 4) //Multicast Promiscuous Enabled
#define RCTL_LPE						(1 << 5) //Long Packet Reception Enable
#define RCTL_LBM_NONE					(0 << 6) //No Loopback
#define RCTL_LBM_PHY					(3 << 6) //PHY or external SerDesc loopback
#define RTCL_RDMTS_HALF					(0 << 8) //Free Buffer Threshold is 1/2 of RDLEN
#define RTCL_RDMTS_QUARTER				(1 << 8) //Free Buffer Threshold is 1/4 of RDLEN
#define RTCL_RDMTS_EIGHTH				(2 << 8) //Free Buffer Threshold is 1/8 of RDLEN
#define RCTL_MO_36						(0 << 12) //Multicast Offset - bits 47:36
#define RCTL_MO_35						(1 << 12) //Multicast Offset - bits 46:35
#define RCTL_MO_34						(2 << 12) //Multicast Offset - bits 45:34
#define RCTL_MO_32						(3 << 12) //Multicast Offset - bits 43:32
#define RCTL_BAM						(1 << 15) //Broadcast Accept Mode
#define RCTL_VFE						(1 << 18) //VLAN Filter Enable
#define RCTL_CFIEN						(1 << 19) //Canonical Form Indicator Enable
#define RCTL_CFI						(1 << 20) //Canonical Form Indicator Bit Value
#define RCTL_DPF						(1 << 22) //Discard Pause Frames
#define RCTL_PMCF						(1 << 23) //Pass MAC Control Frames
#define RCTL_SECRC						(1 << 26) //Strip Ethernet CRC

// Buffer Sizes
#define RCTL_BSIZE_256					(3 << 16)
#define RCTL_BSIZE_512					(2 << 16)
#define RCTL_BSIZE_1024					(1 << 16)
#define RCTL_BSIZE_2048					(0 << 16)
#define RCTL_BSIZE_4096					((3 << 16) | (1 << 25))
#define RCTL_BSIZE_8192					((2 << 16) | (1 << 25))
#define RCTL_BSIZE_16384				((1 << 16) | (1 << 25))

// Transmit Command
#define CMD_EOP							(1 << 0) //End of Packet
#define CMD_IFCS						(1 << 1) //Insert FCS
#define CMD_IC							(1 << 2) //Insert Checksum
#define CMD_RS							(1 << 3) //Report Status
#define CMD_RPS							(1 << 4) //Report Packet Sent
#define CMD_VLE							(1 << 6) //VLAN Packet Enable
#define CMD_IDE							(1 << 7) //Interrupt Delay Enable

// TCTL Register
#define TCTL_EN							(1 << 1) //Transmit Enable
#define TCTL_PSP						(1 << 3) //Pad Short Packets
#define TCTL_CT_SHIFT					4 //Collision Threshold
#define TCTL_COLD_SHIFT					12 //Collision Distance
#define TCTL_SWXOFF						(1 << 22) //Software XOFF Transmission
#define TCTL_RTLC						(1 << 24) //Re-transmit on Late Collision

#define TSTA_DD							(1 << 0) //Descriptor Done
#define TSTA_EC							(1 << 1) //Excess Collisions
#define TSTA_LC							(1 << 2) //Late Collision
#define LSTA_TU							(1 << 3) //Transmit Underrun


#define e1000_NUM_RX_DESC 32
#define e1000_NUM_TX_DESC 8

typedef struct e1000_rx_desc {
	volatile uint64_t addr;
	volatile uint16_t length;
	volatile uint16_t checksum;
	volatile uint8_t status;
	volatile uint8_t errors;
	volatile uint16_t special;
} __attribute__((packed)) e1000_rx_desc_t;
			
typedef struct e1000_tx_desc {
	volatile uint64_t addr;
	volatile uint16_t length;
	volatile uint8_t cso;
	volatile uint8_t cmd;
	volatile uint8_t status;
	volatile uint8_t css;
	volatile uint16_t special;
} __attribute__((packed)) e1000_tx_desc_t;

typedef struct e1000_driver {
	nic_driver_t driver;
	pci_device_t header;
	uint16_t base_port;

	uint16_t io_port;
	uint64_t mem_base;
	uint8_t bar_type;

	bool eerprom_exists;

	uint8_t* tx_ptr;
	uint8_t* rx_ptr;

	struct e1000_rx_desc* rx_descs[e1000_NUM_RX_DESC]; //Receive Descriptor Buffers
	struct e1000_tx_desc* tx_descs[e1000_NUM_TX_DESC]; //Transmit Descriptor Buffers
	uint16_t rx_cur;
	uint16_t tx_cur;
} e1000_driver_t;

nic_driver_t* get_e1000_driver(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function);

bool e1000_is_device_present(driver_t* driver);
char* e1000_get_device_name(driver_t* driver);
void e1000_init(driver_t* driver);

void e1000_send(nic_driver_t* driver, uint8_t* data, uint32_t size);

cpu_registers_t* e1000_interrupt(cpu_registers_t* regs, void* data);
void e1000_receive(e1000_driver_t* driver);
mac_u e1000_get_mac(e1000_driver_t* driver);

void e1000_pci_found(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function);

void e1000_write_command(e1000_driver_t* driver, uint16_t address, uint32_t value);
uint32_t e1000_read_command(e1000_driver_t* driver, uint16_t address);

bool e1000_detect_eeprom(e1000_driver_t* driver);
uint32_t e1000_read_eeprom(e1000_driver_t* driver, uint8_t addr);

void e1000_rx_init(e1000_driver_t* driver);
void e1000_tx_init(e1000_driver_t* driver);

void e1000_start_link(e1000_driver_t* driver);