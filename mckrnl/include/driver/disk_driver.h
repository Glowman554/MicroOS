#pragma once

#include <driver/driver.h>
#include <stdint.h>

typedef struct disk_driver {
	driver_t driver;
	void (*flush)(struct disk_driver*);
	void (*read)(struct disk_driver*, uint64_t sector, uint32_t count, void* buffer);
	void (*write)(struct disk_driver*, uint64_t sector, uint32_t count, void* buffer);
} disk_driver_t;

extern int num_disks;

int register_disk(disk_driver_t* disk);
void read_disk(int disk_id, uint64_t sector, uint32_t count, void* buffer);
void write_disk(int disk_id, uint64_t sector, uint32_t count, void* buffer);
void flush_disk(int disk_id);