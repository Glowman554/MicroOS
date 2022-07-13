#include <driver/disk_driver.h>

#include <config.h>
#include <assert.h>
#include <stdio.h>

int num_disks;

disk_driver_t* disks[MAX_DISKS] = { 0 };

int register_disk(disk_driver_t* disk) {
	assert(num_disks < MAX_DISKS);
	disks[num_disks] = disk;
	debugf("Registered disk %d", num_disks);
	num_disks++;

	return num_disks - 1;
}

void read_disk(int disk_id, uint64_t sector, uint32_t count, void* buffer) {
	assert(disk_id >= 0 && disk_id < MAX_DISKS);
	assert(disks[disk_id] != 0);
	disks[disk_id]->read(disks[disk_id], sector, count, buffer);
}

void write_disk(int disk_id, uint64_t sector, uint32_t count, void* buffer) {
	assert(disk_id >= 0 && disk_id < MAX_DISKS);
	assert(disks[disk_id] != 0);
	disks[disk_id]->write(disks[disk_id], sector, count, buffer);
}

void flush_disk(int disk_id) {
	// assert(disk_id >= 0 && disk_id < MAX_DISKS);
	// assert(disks[disk_id] != 0);
	// disks[disk_id]->flush(disks[disk_id]);
	abortf("The driver should flush when needed!");
}