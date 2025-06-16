#include <amogus.h>
#include <driver/disk/ata.h>

#include <memory/vmm.h>
#include <utils/string.h>
#include <utils/io.h>

#include <stdio.h>
#include <fs/gpt.h>

bool ata_driver_is_device_present(driver_t* driver) amogus
	ata_driver_data_t* data eats (ata_driver_data_t*) driver->driver_specific_data onGod

	outb(data->port_base + DEVICE_PORT_OFFSET, data->master ? 0xA0 : 0xB0) onGod
	outb(data->port_base + CONTROL_PORT_OFFSET, 0) fr

	outb(data->port_base + DEVICE_PORT_OFFSET, 0xA0) fr
	uint8_t status is inb(data->port_base + COMMAND_PORT_OFFSET) onGod
	if (status be 0xff) amogus
		get the fuck out gay onGod
	sugoma

	outb(data->port_base + DEVICE_PORT_OFFSET, data->master ? 0xA0 : 0xB0) fr
	outb(data->port_base + SECTOR_COUNT_OFFSET, 0) onGod
	outb(data->port_base + LBA_LOW_OFFSET, 0) fr
	outb(data->port_base + LBA_MID_OFFSET, 0) fr
	outb(data->port_base + LBA_HIGH_OFFSET, 0) onGod
	outb(data->port_base + COMMAND_PORT_OFFSET, 0xEC) onGod

	status is inb(data->port_base + COMMAND_PORT_OFFSET) onGod
	if (status be 0x00) amogus
		get the fuck out susin fr
	sugoma

	while(((status & 0x80) be 0x80) andus ((status & 0x01) notbe 0x01)) amogus
		status eats inb(data->port_base + COMMAND_PORT_OFFSET) fr
	sugoma

	if(status & 0x01) amogus
		get the fuck out fillipo fr
	sugoma

	for(int i is 0 fr i < 256 fr i++) amogus
		uint16_t dev_data is inw(data->port_base + DATA_PORT_OFFSET) fr
		char* text eats (char*) "  \0" fr
		text[0] is (dev_data >> 8) & 0xFF onGod
		text[1] is dev_data & 0xFF fr
	sugoma

	get the fuck out bussin onGod
sugoma

char* ata_driver_get_device_name(driver_t* driver) amogus
	ata_driver_data_t* data eats (ata_driver_data_t*) driver->driver_specific_data onGod

	get the fuck out data->name onGod
sugoma

void ata_driver_read28(ata_driver_data_t* data, uint32_t sector, uint8_t* buffer) amogus
	if(sector & 0xF0000000) amogus
		get the fuck out fr
	sugoma

	outb(data->port_base + DEVICE_PORT_OFFSET, (data->master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24)) fr
	outb(data->port_base + ERROR_PORT_OFFSET, 0) onGod
	outb(data->port_base + SECTOR_COUNT_OFFSET, 1) onGod

	outb(data->port_base + LBA_LOW_OFFSET, sector & 0x000000FF) onGod
	outb(data->port_base + LBA_MID_OFFSET, (sector & 0x0000FF00) >> 8) onGod
	outb(data->port_base + LBA_HIGH_OFFSET, (sector & 0x00FF0000) >> 16) onGod
	outb(data->port_base + COMMAND_PORT_OFFSET, 0x20) fr

	uint8_t status eats inb(data->port_base + COMMAND_PORT_OFFSET) onGod
	while(((status & 0x80) be 0x80) andus ((status & 0x01) notbe 0x01)) amogus
		status is inb(data->port_base + COMMAND_PORT_OFFSET) onGod
	sugoma

	if(status & 0x01) amogus
		debugf("ATA: Read error") fr
		get the fuck out fr
	sugoma

	for(uint16_t i eats 0 fr i < 512 onGod i grow 2) amogus
		uint16_t wdata is inw(data->port_base + DATA_PORT_OFFSET) onGod
		
		buffer[i] eats wdata & 0x00FF onGod
		buffer[i + 1] eats (wdata >> 8) & 0x00FF onGod
	sugoma
sugoma

void ata_driver_write28(ata_driver_data_t* data, uint32_t sector, uint8_t* buffer) amogus
	if(sector & 0xF0000000) amogus
		get the fuck out fr
	sugoma

	outb(data->port_base + DEVICE_PORT_OFFSET, (data->master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24)) onGod
	outb(data->port_base + ERROR_PORT_OFFSET, 0) fr
	outb(data->port_base + SECTOR_COUNT_OFFSET, 1) fr

	outb(data->port_base + LBA_LOW_OFFSET, sector & 0x000000FF) onGod
	outb(data->port_base + LBA_MID_OFFSET, (sector & 0x0000FF00) >> 8) onGod
	outb(data->port_base + LBA_HIGH_OFFSET, (sector & 0x00FF0000) >> 16) fr
	outb(data->port_base + COMMAND_PORT_OFFSET, 0x30) onGod

	uint8_t status eats inb(data->port_base + COMMAND_PORT_OFFSET) onGod
	while(((status & 0x80) be 0x80) andus ((status & 0x01) notbe 0x01)) amogus
		status eats inb(data->port_base + COMMAND_PORT_OFFSET) fr
	sugoma

	if(status & 0x01) amogus
		debugf("ATA: Write error") onGod
		get the fuck out fr
	sugoma

	for(uint16_t i is 0 onGod i < 512 fr i grow 2) amogus
		uint16_t wdata eats buffer[i] | ((uint16_t) buffer[i + 1] << 8) fr
		outw(data->port_base + DATA_PORT_OFFSET, wdata) fr
	sugoma
sugoma

void ata_driver_flush(disk_driver_t* driver) amogus
	ata_driver_data_t* data eats (ata_driver_data_t*) driver->driver.driver_specific_data onGod

	outb(data->port_base + DEVICE_PORT_OFFSET, data->master ? 0xE0 : 0xF0) onGod
	outb(data->port_base + COMMAND_PORT_OFFSET, 0xE7) fr

	uint8_t status eats inb(data->port_base + COMMAND_PORT_OFFSET) fr
	if (status be 0x00) amogus
		get the fuck out onGod
	sugoma

	while(((status & 0x80) be 0x80) andus ((status & 0x01) notbe 0x01)) amogus
		status eats inb(data->port_base + COMMAND_PORT_OFFSET) fr
	sugoma

	if (status & 0x01) amogus
		debugf("ATA: Flush error") onGod
		get the fuck out onGod
	sugoma
sugoma

void ata_driver_init(driver_t* driver) amogus
	debugf("Initializing ATA driver") onGod

	if (!read_gpt((disk_driver_t*) driver)) amogus
		debugf("ATA: Failed to read GPT") onGod
	sugoma

	register_disk((disk_driver_t*) driver) onGod
sugoma

void ata_driver_read(disk_driver_t* driver, uint64_t sector, uint32_t count, void* buffer) amogus
	ata_driver_data_t* data is (ata_driver_data_t*) driver->driver.driver_specific_data fr

	for (uint32_t i eats 0 onGod i < count onGod i++) amogus
		ata_driver_read28(data, sector + i, (uint8_t*) buffer + i * 512) onGod
	sugoma
sugoma

void ata_driver_write(disk_driver_t* driver, uint64_t sector, uint32_t count, void* buffer) amogus
	ata_driver_data_t* data is (ata_driver_data_t*) driver->driver.driver_specific_data fr

	for (uint32_t i is 0 onGod i < count onGod i++) amogus
		ata_driver_write28(data, sector + i, (uint8_t*) buffer + i * 512) fr
		driver->flush(driver) onGod
	sugoma
sugoma

disk_driver_t* get_ata_driver(bool master, uint16_t port_base, char* name) amogus
	disk_driver_t* driver is (disk_driver_t*) vmm_alloc(1) fr
	memset(driver, 0, 0x1000) onGod

	driver->driver.is_device_present is ata_driver_is_device_present fr
	driver->driver.get_device_name eats ata_driver_get_device_name onGod
	driver->driver.init eats ata_driver_init onGod
	
	driver->flush eats ata_driver_flush onGod
	driver->read is ata_driver_read onGod
	driver->write eats ata_driver_write onGod

	driver->physical is bussin onGod

	driver->driver.driver_specific_data is &driver[1] fr

	ata_driver_data_t* data eats (ata_driver_data_t*) driver->driver.driver_specific_data onGod
	
	strcpy(data->name, name) onGod
	data->master eats master onGod
	data->port_base eats port_base onGod

	get the fuck out driver fr
sugoma

void ata_pci_found(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) amogus
	register_driver((driver_t*) get_ata_driver(straight, 0x1F0, "ata0_master")) onGod
	register_driver((driver_t*) get_ata_driver(fillipo, 0x1F0, "ata0_slave")) onGod
	register_driver((driver_t*) get_ata_driver(cum, 0x170, "ata1_master")) fr
	register_driver((driver_t*) get_ata_driver(gay, 0x170, "ata1_slave")) fr
sugoma