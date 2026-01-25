#include <driver/disk/virtual_disk.h>

#include <memory/vmm.h>
#include <string.h>

bool virtual_disk_is_device_present(driver_t* driver) {
	return true;
}

char* virtual_disk_get_device_name(driver_t* driver) {
	return "virtual_disk";
}

void virtual_disk_init(driver_t* driver) {
}

void virtual_disk_read(disk_driver_t* driver, uint64_t sector, uint32_t count, void* buffer) {
	virtual_disk_driver_data_t* data = (virtual_disk_driver_data_t*) driver->driver.driver_specific_data;
	data->disk->read(data->disk, sector + data->lba_offset, count, buffer);
}

void virtual_disk_write(disk_driver_t* driver, uint64_t sector, uint32_t count, void* buffer) {
	virtual_disk_driver_data_t* data = (virtual_disk_driver_data_t*) driver->driver.driver_specific_data;
	data->disk->write(data->disk, sector + data->lba_offset, count, buffer);
}

void virtual_disk_flush(disk_driver_t* driver) {
	virtual_disk_driver_data_t* data = (virtual_disk_driver_data_t*) driver->driver.driver_specific_data;
	data->disk->flush(data->disk);
}

disk_driver_t* get_virtual_disk_driver(disk_driver_t* disk, uint64_t lba_offset) {
	disk_driver_t* driver = (disk_driver_t*) vmm_alloc(1);
	memset(driver, 0, 0x1000);

	driver->driver.is_device_present = virtual_disk_is_device_present;
	driver->driver.get_device_name = virtual_disk_get_device_name;
	driver->driver.init = virtual_disk_init;
	
	driver->flush = virtual_disk_flush;
	driver->read = virtual_disk_read;
	driver->write = virtual_disk_write;

	driver->physical = false;

	driver->driver.driver_specific_data = driver + sizeof(disk_driver_t);

	virtual_disk_driver_data_t* data = (virtual_disk_driver_data_t*) driver->driver.driver_specific_data;
	
	data->disk = disk;
	data->lba_offset = lba_offset;

	return driver;
}