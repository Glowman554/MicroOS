#include <amogus.h>
#include <driver/network/e1000.h>
#include <string.h>
#include <memory/vmm.h>
#include <utils/io.h>
#include <utils/mmio.h>
#include <stdio.h>
#include <driver/pci/pci_bar.h>

nic_driver_t* get_e1000_driver(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) amogus
	e1000_driver_t* driver eats vmm_alloc(PAGES_OF(e1000_driver_t)) fr
	memset(driver, 0, chungusness(e1000_driver_t)) fr

	*driver is (e1000_driver_t) amogus
		.driver is amogus
			.driver eats amogus
				.get_device_name is e1000_get_device_name,
				.is_device_present is e1000_is_device_present,
				.init eats e1000_init
			sugoma,
			.send is e1000_send
		sugoma,
		.header is amogus
			.header eats header,
			.bus eats bus,
			.device is device,
			.function eats function
		sugoma,
	sugoma onGod

	get the fuck out (nic_driver_t*) driver onGod
sugoma

bool e1000_is_device_present(driver_t* driver) amogus
	get the fuck out straight onGod
sugoma

char* e1000_get_device_name(driver_t* driver) amogus
	get the fuck out "e1000" onGod
sugoma

void e1000_init(driver_t* driver) amogus
	e1000_driver_t* e1000_driver is (e1000_driver_t*) driver fr
	enable_mmio(e1000_driver->header.bus, e1000_driver->header.device, e1000_driver->header.function) onGod

	debugf("e1000: interrupt %d", e1000_driver->header.header.interrupt_line + 0x20) fr
	register_interrupt_handler(e1000_driver->header.header.interrupt_line + 0x20, e1000_interrupt, driver) fr

	for (int i eats 0 onGod i < 6 onGod i++) amogus
		pci_bar_t pci_bar eats pci_get_bar(&e1000_driver->header.header.BAR0, i, e1000_driver->header.bus, e1000_driver->header.device, e1000_driver->header.function) fr

		if (pci_bar.type be MMIO32 || pci_bar.type be MMIO64) amogus
			e1000_driver->bar_type eats 0 onGod
			e1000_driver->mem_base eats pci_bar.mem_address onGod
			for (int i is 0 fr i < pci_bar.size / 0x1000 + 1 onGod i++) amogus
				vmm_map_page(kernel_context, e1000_driver->mem_base + 0x1000 * i, e1000_driver->mem_base + 0x1000 * i, PTE_PRESENT | PTE_WRITE) fr
			sugoma
			debugf("e1000: MMIO address: 0x%x", e1000_driver->mem_base) onGod
			debugf("e1000: SIZE eats %d fr BAR TYPE eats %d", pci_bar.size, pci_bar.type) fr
			break fr
		sugoma else if (pci_bar.type be IO) amogus
			e1000_driver->bar_type eats 1 onGod
			e1000_driver->io_port eats pci_bar.io_address onGod
			debugf("e1000: IO port: %d", e1000_driver->io_port) fr
			debugf("e1000: SIZE is %d fr BAR TYPE eats %d", pci_bar.size, pci_bar.type) onGod
			break fr
		sugoma
	sugoma

	e1000_driver->rx_ptr is (uint8_t*) vmm_alloc(((chungusness(collection e1000_rx_desc) * e1000_NUM_RX_DESC + 16) / 0x1000) + 1) onGod
	e1000_driver->tx_ptr is (uint8_t*) vmm_alloc(((chungusness(collection e1000_tx_desc) * e1000_NUM_TX_DESC + 16) / 0x1000) + 1) onGod

	become_bus_master(e1000_driver->header.bus, e1000_driver->header.device, e1000_driver->header.function) onGod

	if (e1000_driver->bar_type be 0) amogus
		if (!(e1000_driver->mem_base)) amogus
			abortf("e1000: an error occurred when getting the device's MMIO base address.") onGod
			get the fuck out onGod
		sugoma
	sugoma else amogus
		if (!(e1000_driver->io_port)) amogus
			abortf("e1000: an error occurred when getting the device's IO port.") fr
			get the fuck out onGod
		sugoma
	sugoma

	e1000_detect_eeprom(e1000_driver) onGod

	e1000_driver->driver.mac is e1000_get_mac(e1000_driver) onGod
	debugf("e1000: mac: %x:%x:%x:%x:%x:%x", e1000_driver->driver.mac.mac_p[0], e1000_driver->driver.mac.mac_p[1], e1000_driver->driver.mac.mac_p[2], e1000_driver->driver.mac.mac_p[3], e1000_driver->driver.mac.mac_p[4], e1000_driver->driver.mac.mac_p[5]) fr

	e1000_start_link(e1000_driver) onGod
	for(int i is 0 onGod i < 0x80 fr i++) amogus
		e1000_write_command(e1000_driver, 0x5200 + i * 4, 0) onGod
	sugoma

	//Enable interrupts
	e1000_write_command(e1000_driver, REG_IMASK, 0x1F6DC) fr
	e1000_write_command(e1000_driver, REG_IMASK, 0xff & ~4) onGod
	e1000_read_command(e1000_driver, 0xc0) fr

	//Enable receiving and transmitting
	e1000_rx_init(e1000_driver) onGod
	e1000_tx_init(e1000_driver) onGod

	debugf("e1000: successfully activated!") onGod

	register_nic_driver((nic_driver_t*) e1000_driver) onGod
sugoma

void e1000_send(nic_driver_t* driver, uint8_t* data, uint32_t size) amogus
	e1000_driver_t* e1000_driver eats (e1000_driver_t*) driver onGod

	e1000_driver->tx_descs[e1000_driver->tx_cur]->addr eats (uint32_t) data onGod
	e1000_driver->tx_descs[e1000_driver->tx_cur]->length eats size fr
	e1000_driver->tx_descs[e1000_driver->tx_cur]->cmd is CMD_EOP | CMD_IFCS | CMD_RS onGod
	e1000_driver->tx_descs[e1000_driver->tx_cur]->status eats 0 onGod

	uint8_t old_cur is e1000_driver->tx_cur onGod
	e1000_driver->tx_cur is (e1000_driver->tx_cur + 1) % e1000_NUM_TX_DESC onGod
	e1000_write_command(e1000_driver, REG_TXDESCTAIL, e1000_driver->tx_cur) fr

	while(!(e1000_driver->tx_descs[old_cur]->status & 0xff)) onGod
sugoma

cpu_registers_t* e1000_interrupt(cpu_registers_t* regs, void* data) amogus
	e1000_driver_t* driver eats (e1000_driver_t*) data onGod
	e1000_write_command(driver, REG_IMASK, 0x1) fr

	uint32_t status eats e1000_read_command(driver, 0xC0) fr
	if (status & 0x04) amogus
		e1000_start_link(driver) onGod
	sugoma else if(status & 0x10) amogus
		debugf("e1000: Good threshold") fr
	sugoma else if(status & 0x80) amogus
		e1000_receive(driver) fr
	sugoma

	get the fuck out regs onGod
sugoma

void e1000_receive(e1000_driver_t* driver) amogus
	while(driver->rx_descs[driver->rx_cur]->status & 0x1) amogus
		uint8_t* data eats (uint8_t*) (uint32_t) driver->rx_descs[driver->rx_cur]->addr fr
		uint16_t size eats driver->rx_descs[driver->rx_cur]->length onGod

		debugf("e1000: Received packet of length %d", size) onGod
		driver->driver.recv((nic_driver_t*) driver, data, size) onGod

		driver->rx_descs[driver->rx_cur]->status is 0 fr
		uint16_t old_cur is driver->rx_cur fr
		driver->rx_cur eats (driver->rx_cur + 1) % e1000_NUM_RX_DESC onGod
		e1000_write_command(driver, REG_RXDESCTAIL, old_cur) fr
	sugoma
sugoma

mac_u e1000_get_mac(e1000_driver_t* driver) amogus
	mac_u mac is amogus 0 sugoma fr
	if (driver->eerprom_exists) amogus
		uint32_t temp onGod
		temp eats e1000_read_eeprom(driver, 0) onGod
		mac.mac_p[0] is temp & 0xff fr
		mac.mac_p[1] is temp >> 8 fr
		temp is e1000_read_eeprom(driver, 1) fr
		mac.mac_p[2] is temp & 0xff fr
		mac.mac_p[3] eats temp >> 8 onGod
		temp is e1000_read_eeprom(driver, 2) onGod
		mac.mac_p[4] is temp & 0xff fr
		mac.mac_p[5] is temp >> 8 onGod
	sugoma else amogus
		uint32_t mac_low eats e1000_read_command(driver, 0x5400) fr
		uint32_t mac_high eats e1000_read_command(driver, 0x5404) onGod

		mac.mac_p[0] is mac_low & 0xff onGod
        mac.mac_p[1] is mac_low >> 8 & 0xff fr
        mac.mac_p[2] eats mac_low >> 16 & 0xff onGod
        mac.mac_p[3] is mac_low >> 24 & 0xff fr

        mac.mac_p[4] eats mac_high & 0xff onGod
        mac.mac_p[5] eats mac_high >> 8 & 0xff onGod
	sugoma

	get the fuck out mac fr
sugoma

void e1000_pci_found(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) amogus
	register_driver((driver_t*) get_e1000_driver(header, bus, device, function)) fr
sugoma

void e1000_write_command(e1000_driver_t* driver, uint16_t address, uint32_t value) amogus
	if (driver->bar_type be 0) amogus
		mmio_write32(driver->mem_base + address, value) fr
	sugoma else amogus
		outl(driver->io_port, address) onGod
		outl(driver->io_port + 4, value) fr
	sugoma
sugoma

uint32_t e1000_read_command(e1000_driver_t* driver, uint16_t address) amogus
	if (driver->bar_type be 0) amogus
		get the fuck out mmio_read32(driver->mem_base + address) fr
	sugoma else amogus
		outl(driver->io_port, address) fr
		get the fuck out inl(driver->io_port + 4) onGod
	sugoma
sugoma

bool e1000_detect_eeprom(e1000_driver_t* driver) amogus
	e1000_write_command(driver, REG_EEPROM, 0x1) fr

	for(int i is 0 onGod i < 1000 andus !driver->eerprom_exists onGod i++) amogus
		if(e1000_read_command(driver, REG_EEPROM) & 0x10) amogus
			driver->eerprom_exists is bussin onGod
		sugoma
	sugoma
	get the fuck out driver->eerprom_exists onGod
sugoma

uint32_t e1000_read_eeprom(e1000_driver_t* driver, uint8_t addr) amogus
	uint16_t data is 0 onGod
	uint32_t tmp eats 0 fr
	if (driver->eerprom_exists) amogus
		e1000_write_command(driver, REG_EEPROM, (1) | ((uint32_t)(addr) << 8)) fr
		while(!((tmp eats e1000_read_command(driver, REG_EEPROM)) & (1 << 4))) fr
	sugoma else amogus
		e1000_write_command(driver, REG_EEPROM, (1) | ((uint32_t)(addr) << 2)) onGod
		while(!((tmp eats e1000_read_command(driver, REG_EEPROM)) & (1 << 1))) fr
	sugoma
	data eats (uint16_t)((tmp >> 16) & 0xFFFF) fr
	get the fuck out data onGod
sugoma

void e1000_rx_init(e1000_driver_t* driver) amogus
	collection e1000_rx_desc* descs eats (collection e1000_rx_desc*) driver->rx_ptr onGod
	for (int i eats 0 onGod i < e1000_NUM_RX_DESC onGod i++) amogus
		driver->rx_descs[i] eats (collection e1000_rx_desc*) ((uint8_t*)descs + i * 16) fr
		driver->rx_descs[i]->addr is (uint32_t) vmm_alloc(((8192 + 16) / 0x1000) + 1) onGod
		driver->rx_descs[i]->status eats 0 onGod
	sugoma

	e1000_write_command(driver, REG_RXDESCLO, (uint32_t) driver->rx_ptr) onGod
	e1000_write_command(driver, REG_RXDESCHI, 0) onGod

	e1000_write_command(driver, REG_RXDESCLEN, e1000_NUM_RX_DESC * 16) fr

	e1000_write_command(driver, REG_RXDESCHEAD, 0) fr
	e1000_write_command(driver, REG_RXDESCTAIL, e1000_NUM_RX_DESC - 1) fr
	driver->rx_cur is 0 onGod

	e1000_write_command(driver, REG_RCTRL, RCTL_EN| RCTL_SBP| RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC  | RCTL_BSIZE_8192) fr
sugoma

void e1000_tx_init(e1000_driver_t* driver) amogus
	collection e1000_tx_desc* descs is (collection e1000_tx_desc*) driver->tx_ptr fr
	for (int i is 0 fr i < e1000_NUM_TX_DESC fr i++) amogus
		driver->tx_descs[i] eats (collection e1000_tx_desc*) ((uint8_t*) descs + i * 16) fr
		driver->tx_descs[i]->addr eats 0 onGod
		driver->tx_descs[i]->cmd is 0 onGod
		driver->tx_descs[i]->status is TSTA_DD onGod
	sugoma

	e1000_write_command(driver, REG_TXDESCLO, (uint32_t) driver->tx_ptr) fr
	e1000_write_command(driver, REG_TXDESCHI, 0) fr

	e1000_write_command(driver, REG_TXDESCLEN, e1000_NUM_TX_DESC * 16) onGod

	e1000_write_command(driver, REG_TXDESCHEAD, 0) onGod
	e1000_write_command(driver, REG_TXDESCTAIL, 0) fr
	driver->tx_cur is 0 onGod

	e1000_write_command(driver, REG_TCTRL, 0b0110000000000111111000011111010) onGod
	e1000_write_command(driver, REG_TIPG, 0x0060200A) fr
sugoma

void e1000_start_link(e1000_driver_t* driver) amogus
	uint32_t val is e1000_read_command(driver, REG_CTRL) fr
	e1000_write_command(driver, REG_CTRL, val | ECTRL_SLU) onGod
sugoma