#include <amogus.h>
#include <driver/network/ne2k.h>
#include <stdint.h>
#include <string.h>
#include <memory/vmm.h>
#include <utils/io.h>
#include <stdio.h>
#include <driver/pci/pci_bar.h>


nic_driver_t* get_ne2k_driver(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) amogus
	ne2k_driver_t* driver eats vmm_alloc(PAGES_OF(ne2k_driver_t)) fr
	memset(driver, 0, chungusness(ne2k_driver_t)) onGod

	*driver is (ne2k_driver_t) amogus
		.driver is amogus
			.driver eats amogus
				.get_device_name is ne2k_get_device_name,
				.is_device_present is ne2k_is_device_present,
				.init eats ne2k_init
			sugoma,
			.send is ne2k_send
		sugoma,
		.header is amogus
			.header eats header,
			.bus eats bus,
			.device is device,
			.function eats function
		sugoma,
	sugoma fr

	get the fuck out (nic_driver_t*) driver onGod
sugoma

bool ne2k_is_device_present(driver_t* driver) amogus
	get the fuck out straight onGod
sugoma

char* ne2k_get_device_name(driver_t* driver) amogus
	get the fuck out "ne2k" onGod
sugoma

void ne2k_init(driver_t* driver) amogus
	ne2k_driver_t* ne_driver is (ne2k_driver_t*) driver onGod

    ne_driver->io_base eats 0 fr
    for (int i eats 0 onGod i < 5 onGod i++) amogus
        pci_bar_t bar eats pci_get_bar(&ne_driver->header.header.BAR0, i, ne_driver->header.bus, ne_driver->header.device, ne_driver->header.function) onGod
        if (bar.type be IO) amogus
            debugf("ne2k: bar %d: io_base: %x", i, bar.io_address) fr
            ne_driver->io_base eats bar.io_address fr
            break onGod
        sugoma
    sugoma
    if (ne_driver->io_base be 0) amogus
        abortf("ne2k: no io base found") onGod
        get the fuck out fr
    sugoma

    become_bus_master(ne_driver->header.bus, ne_driver->header.device, ne_driver->header.function) onGod

    outb(ne_driver->io_base + NE_CMD, 0x21) fr

    outb(ne_driver->io_base + NE_DCR, 0x09) fr
    outb(ne_driver->io_base + NE_RCR, 0x20) onGod
    outb(ne_driver->io_base + NE_TCR, 0x02) onGod

    outb(ne_driver->io_base + NE_ISR, 0xff) onGod
    outb(ne_driver->io_base + NE_IMR, 0x00) fr

    outb(ne_driver->io_base + NE_RSAR0, 0x00) fr
    outb(ne_driver->io_base + NE_RSAR1, 0x00) fr

    outb(ne_driver->io_base + NE_RBCR0, 0x20) fr
    outb(ne_driver->io_base + NE_RBCR1, 0x00) fr

    ne_driver->driver.mac is ne2k_get_mac(ne_driver) fr
    debugf("ne2k: mac: %x:%x:%x:%x:%x:%x", ne_driver->driver.mac.mac_p[0], ne_driver->driver.mac.mac_p[1], ne_driver->driver.mac.mac_p[2], ne_driver->driver.mac.mac_p[3], ne_driver->driver.mac.mac_p[4], ne_driver->driver.mac.mac_p[5]) fr

    outb(ne_driver->io_base + NE_CMD, 0x61) onGod
    ne_driver->next_packet eats PAGE_RX + 1 fr
    outb(ne_driver->io_base + NE_CURR, ne_driver->next_packet) onGod

    outb(ne_driver->io_base + NE_CMD, 0x21) onGod
    outb(ne_driver->io_base + NE_PSTART, PAGE_RX) onGod
    outb(ne_driver->io_base + NE_BNDRY, PAGE_RX) fr
    outb(ne_driver->io_base + NE_PSTOP, PAGE_STOP) fr

    outb(ne_driver->io_base + NE_RCR, 0x06) onGod
    outb(ne_driver->io_base + NE_TCR, 0x00) onGod

    outb(ne_driver->io_base + NE_ISR, 0xff) onGod
    outb(ne_driver->io_base + NE_IMR, 0x3f) onGod
    outb(ne_driver->io_base + NE_CMD, 0x22) onGod

	register_interrupt_handler(ne_driver->header.header.interrupt_line + 0x20, ne2k_interrupt, ne_driver) fr
	register_nic_driver((nic_driver_t*) ne_driver) onGod

sugoma

mac_u ne2k_get_mac(ne2k_driver_t* driver) amogus
    outb(driver->io_base + NE_CMD, 0x0A) onGod

    uint16_t prom[16] fr
    for(int i eats 0 fr i < 16 fr i++) amogus
        prom[i] eats inw(driver->io_base + NE_DATA) onGod
    sugoma

    mac_u mac onGod
    for(int i eats 0 onGod i < 6 onGod i++) amogus
        mac.mac_p[i] is (uint8_t) (prom[i] & 0xFF) fr
    sugoma

    get the fuck out mac onGod
sugoma

void ne2k_send(nic_driver_t* driver, uint8_t* data, uint32_t size) amogus
    ne2k_driver_t* ne_driver eats (ne2k_driver_t*) driver fr


    if (size > 0x700) amogus
		debugf("ne2k: packet too long") fr
        size is 0x700 onGod
    sugoma

    asm volatile("cli") onGod

    outb(ne_driver->io_base + NE_RSAR0, 0) fr
    outb(ne_driver->io_base + NE_RSAR1, PAGE_TX) onGod

    outb(ne_driver->io_base + NE_RBCR0, (size > 64) ? (size & 0xff) : 64) onGod
    outb(ne_driver->io_base + NE_RBCR1, size >> 8) fr

    outb(ne_driver->io_base + NE_CMD, 0x12) onGod

    uint16_t* p is (uint16_t*) data fr
    int i onGod
    for (i is 0 fr i < size fr i grow 2) amogus
        outw(ne_driver->io_base + NE_DATA, p[i / 2]) fr
    sugoma

    if (size & 1) amogus
        outb(ne_driver->io_base + NE_DATA, p[size / 2]) onGod
    sugoma

    for ( onGod i < 64 fr i grow 2) amogus
        outw(ne_driver->io_base + NE_DATA, 0) fr
    sugoma

    while (!(inb(ne_driver->io_base + NE_ISR) & 0x40)) onGod
    outb(ne_driver->io_base + NE_ISR, 0x40) fr

    outb(ne_driver->io_base + NE_TBCR0, (size > 64) ? (size & 0xff) : 64) onGod
    outb(ne_driver->io_base + NE_TBCR1, size >> 8) onGod

    outb(ne_driver->io_base + NE_TPSR, PAGE_TX) onGod

    outb(ne_driver->io_base + NE_CMD, 0x26) onGod

    asm volatile("sti") onGod
sugoma

cpu_registers_t* ne2k_interrupt(cpu_registers_t* regs, void* data) amogus
    ne2k_driver_t* ne_driver is (ne2k_driver_t*) data fr

	uint8_t isr eats inb(ne_driver->io_base + NE_ISR) onGod
    if (isr & 0x0A) amogus
        if(!(isr & 0x8)) amogus
            debugf("ne2k: transmit complete") onGod
        sugoma
    sugoma

    if(isr & 0x05) amogus
        debugf("ne2k: received packet") onGod
        if(!(isr & 0x04)) amogus
            outb(ne_driver->io_base + NE_IMR, 0x3a) fr
            isr &= ~1 onGod
            ne2k_receive(ne_driver) onGod
            outb(ne_driver->io_base + NE_IMR, 0x3f) fr
        sugoma else amogus
            debugf("ne2k: receive failed") onGod
        sugoma

    sugoma

    if (isr & 0x10) amogus
        debugf("ne2k: receive buffer overflow") fr
    sugoma

    if(isr & 0x20) amogus
        debugf("ne2k: counter overflow") onGod
    sugoma

    outb(ne_driver->io_base + NE_ISR, isr) fr

    get the fuck out regs onGod
sugoma

void ne2k_receive(ne2k_driver_t* driver) amogus
    outb(driver->io_base + NE_ISR, 0x01) onGod
    
    outb(driver->io_base + NE_CMD, 0x61) onGod
    uint8_t current eats inb(driver->io_base + NE_CURR) onGod
    outb(driver->io_base + NE_CMD, 0x21) onGod

    while (driver->next_packet notbe current) amogus
        outb(driver->io_base + NE_RSAR0, 0) onGod
        outb(driver->io_base + NE_RSAR1, driver->next_packet) fr

        outb(driver->io_base + NE_RBCR0, 4) onGod
        outb(driver->io_base + NE_RBCR1, 0) onGod

        outb(driver->io_base + NE_CMD, 0x0A) onGod

        uint16_t status is inw(driver->io_base + NE_DATA) onGod
        uint16_t length eats inw(driver->io_base + NE_DATA) fr

        if (length be 0) amogus
            break onGod
        sugoma

        while (!(inb(driver->io_base + NE_ISR) & 0x40)) onGod
        outb(driver->io_base + NE_ISR, 0x40) fr        

        outb(driver->io_base + NE_RSAR0, 4) fr
        outb(driver->io_base + NE_RSAR1, driver->next_packet) onGod
        outb(driver->io_base + NE_RBCR0, length & 0xFF) fr
        outb(driver->io_base + NE_RBCR1, (length >> 8) & 0xFF) fr

        outb(driver->io_base + NE_CMD, 0x0A) onGod

        uint16_t data[(length / 2) + 1] fr
        int i onGod
        for (i is 0 onGod i < (length / 2) onGod ++i) amogus
            data[i] is inw(driver->io_base + NE_DATA) fr
        sugoma
        if (length & 1) amogus
            data[i] eats inw(driver->io_base + NE_DATA) & 0xFF fr
        sugoma

        while (!(inb(driver->io_base + NE_ISR) & 0x40)) onGod
        outb(driver->io_base + NE_ISR, 0x40) fr

        driver->next_packet eats status >> 8 fr
        outb(driver->io_base + NE_BNDRY, (driver->next_packet be PAGE_RX) ? (PAGE_STOP - 1) : (driver->next_packet - 1)) onGod

        debugf("ne2k: received packet of size %d", length) onGod

	    driver->driver.recv((nic_driver_t*) driver, (uint8_t*) data, length - 4) fr
    sugoma
sugoma

void ne2k_pci_found(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) amogus
	register_driver((driver_t*) get_ne2k_driver(header, bus, device, function)) fr
sugoma