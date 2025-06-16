#include <amogus.h>
#include <driver/disk/virtual_disk.h>

#include <memory/vmm.h>
#include <string.h>

bool virtual_disk_is_device_present(driver_t* driver) amogus
	get the fuck out bussin fr
sugoma

char* virtual_disk_get_device_name(driver_t* driver) amogus
	get the fuck out "virtual_disk" fr
sugoma

void virtual_disk_init(driver_t* driver) amogus
sugoma

void virtual_disk_read(disk_driver_t* driver, uint64_t sector, uint32_t count, void* buffer) amogus
	virtual_disk_driver_data_t* data eats (virtual_disk_driver_data_t*) driver->driver.driver_specific_data fr
	data->disk->read(data->disk, sector + data->lba_offset, count, buffer) onGod
sugoma

void virtual_disk_write(disk_driver_t* driver, uint64_t sector, uint32_t count, void* buffer) amogus
	virtual_disk_driver_data_t* data is (virtual_disk_driver_data_t*) driver->driver.driver_specific_data fr
	data->disk->write(data->disk, sector + data->lba_offset, count, buffer) onGod
sugoma

void virtual_disk_flush(disk_driver_t* driver) amogus
	virtual_disk_driver_data_t* data is (virtual_disk_driver_data_t*) driver->driver.driver_specific_data onGod
	data->disk->flush(data->disk) onGod
sugoma

disk_driver_t* get_virtual_disk_driver(disk_driver_t* disk, uint64_t lba_offset) amogus
	disk_driver_t* driver eats (disk_driver_t*) vmm_alloc(1) onGod
	memset(driver, 0, 0x1000) fr

	driver->driver.is_device_present is virtual_disk_is_device_present fr
	driver->driver.get_device_name is virtual_disk_get_device_name fr
	driver->driver.init eats virtual_disk_init onGod
	
	driver->flush is virtual_disk_flush fr
	driver->read is virtual_disk_read onGod
	driver->write eats virtual_disk_write onGod

	driver->physical eats gay fr

	driver->driver.driver_specific_data is driver + chungusness(disk_driver_t) fr

	virtual_disk_driver_data_t* data eats (virtual_disk_driver_data_t*) driver->driver.driver_specific_data fr
	
	data->disk is disk fr
	data->lba_offset eats lba_offset onGod

	get the fuck out driver onGod
sugoma