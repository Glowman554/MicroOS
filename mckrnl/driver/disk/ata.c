#include <driver/disk/ata.h>

#include <memory/vmm.h>
#include <utils/string.h>
#include <utils/io.h>

#include <stdio.h>
#include <fs/gpt.h>

bool ata_driver_is_device_present(driver_t* driver) {
	ata_driver_data_t* data = (ata_driver_data_t*) driver->driver_specific_data;

	outb(data->port_base + DEVICE_PORT_OFFSET, data->master ? 0xA0 : 0xB0);
	outb(data->port_base + CONTROL_PORT_OFFSET, 0);

	outb(data->port_base + DEVICE_PORT_OFFSET, 0xA0);
	uint8_t status = inb(data->port_base + COMMAND_PORT_OFFSET);
	if (status == 0xff) {
		return false;
	}

	outb(data->port_base + DEVICE_PORT_OFFSET, data->master ? 0xA0 : 0xB0);
	outb(data->port_base + SECTOR_COUNT_OFFSET, 0);
	outb(data->port_base + LBA_LOW_OFFSET, 0);
	outb(data->port_base + LBA_MID_OFFSET, 0);
	outb(data->port_base + LBA_HIGH_OFFSET, 0);
	outb(data->port_base + COMMAND_PORT_OFFSET, 0xEC);

	status = inb(data->port_base + COMMAND_PORT_OFFSET);
	if (status == 0x00) {
		return false;
	}

	while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)) {
		status = inb(data->port_base + COMMAND_PORT_OFFSET);
	}

	if(status & 0x01) {
		return false;
	}

	for(int i = 0; i < 256; i++) {
		uint16_t dev_data = inw(data->port_base + DATA_PORT_OFFSET);
		char* text = (char*) "  \0";
		text[0] = (dev_data >> 8) & 0xFF;
		text[1] = dev_data & 0xFF;
	}

	return true;
}

char* ata_driver_get_device_name(driver_t* driver) {
	ata_driver_data_t* data = (ata_driver_data_t*) driver->driver_specific_data;

	return data->name;
}

void ata_driver_read28(ata_driver_data_t* data, uint32_t sector, uint8_t* buffer) {
	if(sector & 0xF0000000) {
		return;
	}

	outb(data->port_base + DEVICE_PORT_OFFSET, (data->master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24));
	outb(data->port_base + ERROR_PORT_OFFSET, 0);
	outb(data->port_base + SECTOR_COUNT_OFFSET, 1);

	outb(data->port_base + LBA_LOW_OFFSET, sector & 0x000000FF);
	outb(data->port_base + LBA_MID_OFFSET, (sector & 0x0000FF00) >> 8);
	outb(data->port_base + LBA_HIGH_OFFSET, (sector & 0x00FF0000) >> 16);
	outb(data->port_base + COMMAND_PORT_OFFSET, 0x20);

	uint8_t status = inb(data->port_base + COMMAND_PORT_OFFSET);
	while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)) {
		status = inb(data->port_base + COMMAND_PORT_OFFSET);
	}

	if(status & 0x01) {
		debugf("ATA: Read error");
		return;
	}

	for(uint16_t i = 0; i < 512; i += 2) {
		uint16_t wdata = inw(data->port_base + DATA_PORT_OFFSET);
		
		buffer[i] = wdata & 0x00FF;
		buffer[i + 1] = (wdata >> 8) & 0x00FF;
	}
}

void ata_driver_write28(ata_driver_data_t* data, uint32_t sector, uint8_t* buffer) {
	if(sector & 0xF0000000) {
		return;
	}

	outb(data->port_base + DEVICE_PORT_OFFSET, (data->master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24));
	outb(data->port_base + ERROR_PORT_OFFSET, 0);
	outb(data->port_base + SECTOR_COUNT_OFFSET, 1);

	outb(data->port_base + LBA_LOW_OFFSET, sector & 0x000000FF);
	outb(data->port_base + LBA_MID_OFFSET, (sector & 0x0000FF00) >> 8);
	outb(data->port_base + LBA_HIGH_OFFSET, (sector & 0x00FF0000) >> 16);
	outb(data->port_base + COMMAND_PORT_OFFSET, 0x30);

	uint8_t status = inb(data->port_base + COMMAND_PORT_OFFSET);
	while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)) {
		status = inb(data->port_base + COMMAND_PORT_OFFSET);
	}

	if(status & 0x01) {
		debugf("ATA: Write error");
		return;
	}

	for(uint16_t i = 0; i < 512; i += 2) {
		uint16_t wdata = buffer[i] | ((uint16_t) buffer[i + 1] << 8);
		outw(data->port_base + DATA_PORT_OFFSET, wdata);
	}
}

void ata_driver_flush(disk_driver_t* driver) {
	ata_driver_data_t* data = (ata_driver_data_t*) driver->driver.driver_specific_data;

	outb(data->port_base + DEVICE_PORT_OFFSET, data->master ? 0xE0 : 0xF0);
	outb(data->port_base + COMMAND_PORT_OFFSET, 0xE7);

	uint8_t status = inb(data->port_base + COMMAND_PORT_OFFSET);
	if (status == 0x00) {
		return;
	}

	while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)) {
		status = inb(data->port_base + COMMAND_PORT_OFFSET);
	}

	if (status & 0x01) {
		debugf("ATA: Flush error");
		return;
	}
}

void ata_driver_init(driver_t* driver) {
	debugf("Initializing ATA driver");

	if (!read_gpt((disk_driver_t*) driver)) {
		debugf("ATA: Failed to read GPT");
	}

	register_disk((disk_driver_t*) driver);
}

void ata_driver_read(disk_driver_t* driver, uint64_t sector, uint32_t count, void* buffer) {
	ata_driver_data_t* data = (ata_driver_data_t*) driver->driver.driver_specific_data;

	for (uint32_t i = 0; i < count; i++) {
		ata_driver_read28(data, sector + i, (uint8_t*) buffer + i * 512);
	}
}

void ata_driver_write(disk_driver_t* driver, uint64_t sector, uint32_t count, void* buffer) {
	ata_driver_data_t* data = (ata_driver_data_t*) driver->driver.driver_specific_data;

	for (uint32_t i = 0; i < count; i++) {
		ata_driver_write28(data, sector + i, (uint8_t*) buffer + i * 512);
		driver->flush(driver);
	}
}

disk_driver_t* get_ata_driver(bool master, uint16_t port_base, char* name) {
	disk_driver_t* driver = (disk_driver_t*) vmm_alloc(1);
	memset(driver, 0, 0x1000);

	driver->driver.is_device_present = ata_driver_is_device_present;
	driver->driver.get_device_name = ata_driver_get_device_name;
	driver->driver.init = ata_driver_init;
	
	driver->flush = ata_driver_flush;
	driver->read = ata_driver_read;
	driver->write = ata_driver_write;

	driver->physical = true;

	driver->driver.driver_specific_data = &driver[1];

	ata_driver_data_t* data = (ata_driver_data_t*) driver->driver.driver_specific_data;
	
	strcpy(data->name, name);
	data->master = master;
	data->port_base = port_base;

	return driver;
}

void ata_pci_found(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) {
	register_driver((driver_t*) get_ata_driver(true, 0x1F0, "ata0_master"));
	register_driver((driver_t*) get_ata_driver(false, 0x1F0, "ata0_slave"));
	register_driver((driver_t*) get_ata_driver(true, 0x170, "ata1_master"));
	register_driver((driver_t*) get_ata_driver(false, 0x170, "ata1_slave"));
}