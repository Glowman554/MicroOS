#pragma once

#include <driver/driver.h>

typedef struct disk_driver {
	driver_t driver;
	void (*flush)(struct disk_driver*);
	void (*read)(struct disk_driver*, uint64_t sector, uint32_t count, void* buffer);
	void (*write)(struct disk_driver*, uint64_t sector, uint32_t count, void* buffer);
} disk_driver_t;