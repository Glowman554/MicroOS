#pragma once

#include <driver/disk_driver.h>

#include <stdbool.h>

typedef struct ata_driver_data {
	char name[32];
	bool master;
	uint16_t port_base;
} ata_driver_data_t;

#define DATA_PORT_OFFSET 0x0
#define ERROR_PORT_OFFSET 0x1
#define SECTOR_COUNT_OFFSET 0x2
#define LBA_LOW_OFFSET 0x3
#define LBA_MID_OFFSET 0x4
#define LBA_HIGH_OFFSET 0x5
#define DEVICE_PORT_OFFSET 0x6
#define COMMAND_PORT_OFFSET 0x7
#define CONTROL_PORT_OFFSET 0x206

disk_driver_t* get_ata_driver(bool master, uint16_t port_base, char* name);