#include <driver/disk_driver.h>

#include <config.h>
#include <assert.h>
#include <stdio.h>
#include <memory/vmm.h>
#include <stddef.h>

int num_disks = 0;
disk_driver_t** disks = NULL;

int register_disk(disk_driver_t* disk) {
	disks = vmm_resize(sizeof(disk_driver_t*), num_disks, num_disks + 1, disks);
	disks[num_disks] = disk;
	debugf("Registered disk %d", num_disks);
	num_disks++;

	return num_disks - 1;
}

void read_disk(int disk_id, uint64_t sector, uint32_t count, void* buffer) {
	assert(disk_id >= 0 && disk_id < num_disks);
	assert(disks[disk_id] != 0);
	disks[disk_id]->read(disks[disk_id], sector, count, buffer);
}

void write_disk(int disk_id, uint64_t sector, uint32_t count, void* buffer) {
	assert(disk_id >= 0 && disk_id < num_disks);
	assert(disks[disk_id] != 0);
	disks[disk_id]->write(disks[disk_id], sector, count, buffer);
}

void flush_disk(int disk_id) {
	// assert(disk_id >= 0 && disk_id < MAX_DISKS);
	// assert(disks[disk_id] != 0);
	// disks[disk_id]->flush(disks[disk_id]);
	abortf(false, "The driver should flush when needed!");
}

bool is_disk_physical(int disk_id) {
	assert(disk_id >= 0 && disk_id < num_disks);
	assert(disks[disk_id] != 0);
	return disks[disk_id]->physical;
}