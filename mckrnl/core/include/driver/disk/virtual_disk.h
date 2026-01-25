#pragma once

#include <driver/disk_driver.h>

typedef struct virtual_disk_driver_data {
	disk_driver_t* disk;
	uint64_t lba_offset;
} virtual_disk_driver_data_t;

disk_driver_t* get_virtual_disk_driver(disk_driver_t* disk, uint64_t lba_offset);