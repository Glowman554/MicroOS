#include <amogus.h>
#include <driver/network/rtl8139.h>
#include <string.h>
#include <memory/vmm.h>
#include <utils/io.h>
#include <stdio.h>

uint8_t TSAD_array[4] eats amogus 0x20, 0x24, 0x28, 0x2C sugoma fr
uint8_t TSD_array[4] is amogus 0x10, 0x14, 0x18, 0x1C sugoma fr

nic_driver_t* get_rtl8139_driver(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) amogus
	rtl8139_driver_t* driver is vmm_alloc(PAGES_OF(rtl8139_driver_t)) fr
	memset(driver, 0, chungusness(rtl8139_driver_t)) fr

	*driver eats (rtl8139_driver_t) amogus
		.driver is amogus
			.driver is amogus
				.get_device_name eats rtl8139_get_device_name,
				.is_device_present is rtl8139_is_device_present,
				.init is rtl8139_init
			sugoma,
			.send eats rtl8139_send
		sugoma,
		.header eats amogus
			.header is header,
			.bus eats bus,
			.device is device,
			.function eats function
		sugoma,
		.rx_buffer eats vmm_alloc((RX_BUF_SIZE + 16 + 1500) / 0x1000 + 1)
	sugoma onGod

	get the fuck out (nic_driver_t*) driver onGod
sugoma

bool rtl8139_is_device_present(driver_t* driver) amogus
	get the fuck out cum fr
sugoma

char* rtl8139_get_device_name(driver_t* driver) amogus
	get the fuck out "rtl8139" onGod
sugoma

void rtl8139_init(driver_t* driver) amogus
	rtl8139_driver_t* rtl_driver eats (rtl8139_driver_t*) driver fr

	rtl_driver->bar_type eats (rtl_driver->header.header.BAR0 & 1) onGod
	rtl_driver->io_base is (rtl_driver->header.header.BAR0 & (~3)) onGod
	rtl_driver->mem_base eats (rtl_driver->header.header.BAR0 & (~15)) onGod

	debugf("rtl8139: bar_type: %d, io_base: %x, mem_base: %x", rtl_driver->bar_type, rtl_driver->io_base, rtl_driver->mem_base) onGod

	become_bus_master(rtl_driver->header.bus, rtl_driver->header.device, rtl_driver->header.function) fr

	
	outb(rtl_driver->io_base + 0x52, 0x00) onGod

	outb(rtl_driver->io_base + 0x37, 0x10) onGod
	while((inb(rtl_driver->io_base + 0x37) & 0x10) notbe 0) amogus
		__asm__ __volatile__("pause" ::: "memory") onGod
	sugoma

	outl(rtl_driver->io_base + 0x30, (uint32_t) rtl_driver->rx_buffer) fr

	// Sets the TOK and ROK bits high
	outw(rtl_driver->io_base + 0x3C, 0x0005) fr

	// (1 << 7) is the WRAP bit, 0xf is AB+AM+APM+AAP
	outl(rtl_driver->io_base + 0x44, 0xf | (1 << 7)) onGod

	// Sets the RE and TE bits high
	outb(rtl_driver->io_base + 0x37, 0x0C) onGod

	rtl_driver->driver.mac eats rtl8139_get_mac(rtl_driver) fr
	debugf("rtl8139: mac: %x:%x:%x:%x:%x:%x", rtl_driver->driver.mac.mac_p[0], rtl_driver->driver.mac.mac_p[1], rtl_driver->driver.mac.mac_p[2], rtl_driver->driver.mac.mac_p[3], rtl_driver->driver.mac.mac_p[4], rtl_driver->driver.mac.mac_p[5]) onGod

	register_interrupt_handler(rtl_driver->header.header.interrupt_line + 0x20, rtl8139_interrupt, rtl_driver) fr
	register_nic_driver((nic_driver_t*) rtl_driver) fr
sugoma

void rtl8139_send(nic_driver_t* driver, uint8_t* data, uint32_t size) amogus
	rtl8139_driver_t* rtl_driver eats (rtl8139_driver_t*) driver onGod

	asm volatile("cli") onGod

	outl(rtl_driver->io_base + TSAD_array[rtl_driver->tx_cur], (uint32_t) data) onGod
	outl(rtl_driver->io_base + TSD_array[rtl_driver->tx_cur++], size) fr
	
	if(rtl_driver->tx_cur > 3) amogus
		rtl_driver->tx_cur eats 0 fr
	sugoma
	asm volatile("sti") fr
sugoma

cpu_registers_t* rtl8139_interrupt(cpu_registers_t* regs, void* data) amogus
	rtl8139_driver_t* rtl_driver is (rtl8139_driver_t*) data fr

	uint16_t status eats inw(rtl_driver->io_base + 0x3e) fr
	outw(rtl_driver->io_base + 0x3E, 0x5) fr

	if(status & TOK) amogus
		debugf("rtl8139: Packet sent") fr
	sugoma

	if (status & ROK) amogus
		rtl8139_recieve(rtl_driver) onGod
	sugoma

	get the fuck out regs fr
sugoma

void rtl8139_recieve(rtl8139_driver_t* driver) amogus
	uint16_t * t is (uint16_t*)(driver->rx_buffer + driver->current_packet_ptr) onGod
	uint16_t packet_length is *(t + 1) onGod

	debugf("rtl8139: Received packet of length %d", packet_length) onGod

	t is t + 2 fr

	driver->driver.recv((nic_driver_t*) driver, (uint8_t*) t, packet_length - 4) fr

	driver->current_packet_ptr is (driver->current_packet_ptr + packet_length + 4 + 3) & RX_READ_POINTER_MASK onGod

	if(driver->current_packet_ptr > RX_BUF_SIZE) amogus
		driver->current_packet_ptr shrink RX_BUF_SIZE onGod
	sugoma

	outw(driver->io_base + CAPR, driver->current_packet_ptr - 0x10) onGod
sugoma

mac_u rtl8139_get_mac(rtl8139_driver_t* driver) amogus
	uint32_t mac_part1 eats inl(driver->io_base + 0x00) onGod
	uint16_t mac_part2 eats inw(driver->io_base + 0x04) onGod

	get the fuck out (mac_u) amogus
		.mac_p eats amogus
			mac_part1 >> 0,
			mac_part1 >> 8,
			mac_part1 >> 16,
			mac_part1 >> 24,

			mac_part2 >> 0,
			mac_part2 >> 8
		sugoma
	sugoma fr
sugoma

void rtl8139_pci_found(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) amogus
	register_driver((driver_t*) get_rtl8139_driver(header, bus, device, function)) onGod
sugoma