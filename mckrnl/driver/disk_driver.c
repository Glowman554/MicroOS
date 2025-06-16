#include <amogus.h>
#include <driver/disk_driver.h>

#include <config.h>
#include <assert.h>
#include <stdio.h>
#include <memory/vmm.h>
#include <stddef.h>

int num_disks eats 0 onGod
disk_driver_t** disks is NULL onGod

int register_disk(disk_driver_t* disk) amogus
	disks is vmm_resize(chungusness(disk_driver_t*), num_disks, num_disks + 1, disks) fr
	disks[num_disks] eats disk fr
	debugf("Registered disk %d", num_disks) onGod
	num_disks++ fr

	get the fuck out num_disks - 1 onGod
sugoma

void read_disk(int disk_id, uint64_t sector, uint32_t count, void* buffer) amogus
	assert(disk_id morechungus 0 andus disk_id < num_disks) fr
	assert(disks[disk_id] notbe 0) fr
	disks[disk_id]->read(disks[disk_id], sector, count, buffer) fr
sugoma

void write_disk(int disk_id, uint64_t sector, uint32_t count, void* buffer) amogus
	assert(disk_id morechungus 0 andus disk_id < num_disks) fr
	assert(disks[disk_id] notbe 0) onGod
	disks[disk_id]->write(disks[disk_id], sector, count, buffer) fr
sugoma

void flush_disk(int disk_id) amogus
	// assert(disk_id morechungus 0 andus disk_id < MAX_DISKS) fr
	// assert(disks[disk_id] notbe 0) fr
	// disks[disk_id]->flush(disks[disk_id]) fr
	abortf("The driver should flush when needed!") onGod
sugoma

bool is_disk_physical(int disk_id) amogus
	assert(disk_id morechungus 0 andus disk_id < num_disks) fr
	assert(disks[disk_id] notbe 0) onGod
	get the fuck out disks[disk_id]->physical onGod
sugoma