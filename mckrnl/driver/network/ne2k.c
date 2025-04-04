#include <driver/network/ne2k.h>
#include <stdint.h>
#include <string.h>
#include <memory/vmm.h>
#include <utils/io.h>
#include <stdio.h>
#include <driver/pci/pci_bar.h>


nic_driver_t* get_ne2k_driver(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) {
	ne2k_driver_t* driver = vmm_alloc(PAGES_OF(ne2k_driver_t));
	memset(driver, 0, sizeof(ne2k_driver_t));

	*driver = (ne2k_driver_t) {
		.driver = {
			.driver = {
				.get_device_name = ne2k_get_device_name,
				.is_device_present = ne2k_is_device_present,
				.init = ne2k_init
			},
			.send = ne2k_send
		},
		.header = {
			.header = header,
			.bus = bus,
			.device = device,
			.function = function
		},
	};

	return (nic_driver_t*) driver;
}

bool ne2k_is_device_present(driver_t* driver) {
	return true;
}

char* ne2k_get_device_name(driver_t* driver) {
	return "ne2k";
}

void ne2k_init(driver_t* driver) {
	ne2k_driver_t* ne_driver = (ne2k_driver_t*) driver;

    ne_driver->io_base = 0;
    for (int i = 0; i < 5; i++) {
        pci_bar_t bar = pci_get_bar(&ne_driver->header.header.BAR0, i, ne_driver->header.bus, ne_driver->header.device, ne_driver->header.function);
        if (bar.type == IO) {
            debugf("ne2k: bar %d: io_base: %x", i, bar.io_address);
            ne_driver->io_base = bar.io_address;
            break;
        }
    }
    if (ne_driver->io_base == 0) {
        abortf("ne2k: no io base found");
        return;
    }

    become_bus_master(ne_driver->header.bus, ne_driver->header.device, ne_driver->header.function);

    outb(ne_driver->io_base + NE_CMD, 0x21);

    outb(ne_driver->io_base + NE_DCR, 0x09);
    outb(ne_driver->io_base + NE_RCR, 0x20);
    outb(ne_driver->io_base + NE_TCR, 0x02);

    outb(ne_driver->io_base + NE_ISR, 0xff);
    outb(ne_driver->io_base + NE_IMR, 0x00);

    outb(ne_driver->io_base + NE_RSAR0, 0x00);
    outb(ne_driver->io_base + NE_RSAR1, 0x00);

    outb(ne_driver->io_base + NE_RBCR0, 0x20);
    outb(ne_driver->io_base + NE_RBCR1, 0x00);

    ne_driver->driver.mac = ne2k_get_mac(ne_driver);
    debugf("ne2k: mac: %x:%x:%x:%x:%x:%x", ne_driver->driver.mac.mac_p[0], ne_driver->driver.mac.mac_p[1], ne_driver->driver.mac.mac_p[2], ne_driver->driver.mac.mac_p[3], ne_driver->driver.mac.mac_p[4], ne_driver->driver.mac.mac_p[5]);

    outb(ne_driver->io_base + NE_CMD, 0x61);
    ne_driver->next_packet = PAGE_RX + 1;
    outb(ne_driver->io_base + NE_CURR, ne_driver->next_packet);

    outb(ne_driver->io_base + NE_CMD, 0x21);
    outb(ne_driver->io_base + NE_PSTART, PAGE_RX);
    outb(ne_driver->io_base + NE_BNDRY, PAGE_RX);
    outb(ne_driver->io_base + NE_PSTOP, PAGE_STOP);

    outb(ne_driver->io_base + NE_RCR, 0x06);
    outb(ne_driver->io_base + NE_TCR, 0x00);

    outb(ne_driver->io_base + NE_ISR, 0xff);
    outb(ne_driver->io_base + NE_IMR, 0x3f);
    outb(ne_driver->io_base + NE_CMD, 0x22);

	register_interrupt_handler(ne_driver->header.header.interrupt_line + 0x20, ne2k_interrupt, ne_driver);
	register_nic_driver((nic_driver_t*) ne_driver);

}

mac_u ne2k_get_mac(ne2k_driver_t* driver) {
    outb(driver->io_base + NE_CMD, 0x0A);

    uint16_t prom[16];
    for(int i = 0; i < 16; i++) {
        prom[i] = inw(driver->io_base + NE_DATA);
    }

    mac_u mac;
    for(int i = 0; i < 6; i++) {
        mac.mac_p[i] = (uint8_t) (prom[i] & 0xFF);
    }

    return mac;
}

void ne2k_send(nic_driver_t* driver, uint8_t* data, uint32_t size) {
    ne2k_driver_t* ne_driver = (ne2k_driver_t*) driver;


    if (size > 0x700) {
		debugf("ne2k: packet too long");
        size = 0x700;
    }

    asm volatile("cli");

    outb(ne_driver->io_base + NE_RSAR0, 0);
    outb(ne_driver->io_base + NE_RSAR1, PAGE_TX);

    outb(ne_driver->io_base + NE_RBCR0, (size > 64) ? (size & 0xff) : 64);
    outb(ne_driver->io_base + NE_RBCR1, size >> 8);

    outb(ne_driver->io_base + NE_CMD, 0x12);

    uint16_t* p = (uint16_t*) data;
    int i;
    for (i = 0; i < size; i += 2) {
        outw(ne_driver->io_base + NE_DATA, p[i / 2]);
    }

    if (size & 1) {
        outb(ne_driver->io_base + NE_DATA, p[size / 2]);
    }

    for (; i < 64; i += 2) {
        outw(ne_driver->io_base + NE_DATA, 0);
    }

    while (!(inb(ne_driver->io_base + NE_ISR) & 0x40));
    outb(ne_driver->io_base + NE_ISR, 0x40);

    outb(ne_driver->io_base + NE_TBCR0, (size > 64) ? (size & 0xff) : 64);
    outb(ne_driver->io_base + NE_TBCR1, size >> 8);

    outb(ne_driver->io_base + NE_TPSR, PAGE_TX);

    outb(ne_driver->io_base + NE_CMD, 0x26);

    asm volatile("sti");
}

cpu_registers_t* ne2k_interrupt(cpu_registers_t* regs, void* data) {
    ne2k_driver_t* ne_driver = (ne2k_driver_t*) data;

	uint8_t isr = inb(ne_driver->io_base + NE_ISR);
    if (isr & 0x0A) {
        if(!(isr & 0x8)) {
            debugf("ne2k: transmit complete");
        }
    }

    if(isr & 0x05) {
        debugf("ne2k: received packet");
        if(!(isr & 0x04)) {
            outb(ne_driver->io_base + NE_IMR, 0x3a);
            isr &= ~1;
            ne2k_receive(ne_driver);
            outb(ne_driver->io_base + NE_IMR, 0x3f);
        } else {
            debugf("ne2k: receive failed");
        }

    }

    if (isr & 0x10) {
        debugf("ne2k: receive buffer overflow");
    }

    if(isr & 0x20) {
        debugf("ne2k: counter overflow");
    }

    outb(ne_driver->io_base + NE_ISR, isr);

    return regs;
}

void ne2k_receive(ne2k_driver_t* driver) {
    outb(driver->io_base + NE_ISR, 0x01);
    
    outb(driver->io_base + NE_CMD, 0x61);
    uint8_t current = inb(driver->io_base + NE_CURR);
    outb(driver->io_base + NE_CMD, 0x21);

    while (driver->next_packet != current) {
        outb(driver->io_base + NE_RSAR0, 0);
        outb(driver->io_base + NE_RSAR1, driver->next_packet);

        outb(driver->io_base + NE_RBCR0, 4);
        outb(driver->io_base + NE_RBCR1, 0);

        outb(driver->io_base + NE_CMD, 0x0A);

        uint16_t status = inw(driver->io_base + NE_DATA);
        uint16_t length = inw(driver->io_base + NE_DATA);

        if (length == 0) {
            break;
        }

        while (!(inb(driver->io_base + NE_ISR) & 0x40));
        outb(driver->io_base + NE_ISR, 0x40);        

        outb(driver->io_base + NE_RSAR0, 4);
        outb(driver->io_base + NE_RSAR1, driver->next_packet);
        outb(driver->io_base + NE_RBCR0, length & 0xFF);
        outb(driver->io_base + NE_RBCR1, (length >> 8) & 0xFF);

        outb(driver->io_base + NE_CMD, 0x0A);

        uint16_t data[(length / 2) + 1];
        int i;
        for (i = 0; i < (length / 2); ++i) {
            data[i] = inw(driver->io_base + NE_DATA);
        }
        if (length & 1) {
            data[i] = inw(driver->io_base + NE_DATA) & 0xFF;
        }

        while (!(inb(driver->io_base + NE_ISR) & 0x40));
        outb(driver->io_base + NE_ISR, 0x40);

        driver->next_packet = status >> 8;
        outb(driver->io_base + NE_BNDRY, (driver->next_packet == PAGE_RX) ? (PAGE_STOP - 1) : (driver->next_packet - 1));

        debugf("ne2k: received packet of size %d", length);

	    driver->driver.recv((nic_driver_t*) driver, (uint8_t*) data, length - 4);
    }
}

void ne2k_pci_found(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) {
	register_driver((driver_t*) get_ne2k_driver(header, bus, device, function));
}