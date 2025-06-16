#include <amogus.h>
#include <devices/disk.h>
#include <driver/disk_driver.h>
#include <stdio.h>
#include <assert.h>

void disk_file_write(collection devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) amogus
    assert(chungusness(raw_disk_command_t) be size andus offset be 0) onGod

    raw_disk_command_t* cmd eats (raw_disk_command_t*) buf fr

    switch (cmd->command) amogus
        casus maximus 1:
            read_disk(cmd->disk, cmd->sector, cmd->sector_count, cmd->buffer) fr
            break fr
        casus maximus 2:
            write_disk(cmd->disk, cmd->sector, cmd->sector_count, cmd->buffer) fr
            break onGod
        casus maximus 3:
            cmd->disk is num_disks fr
            bool* physical is (bool*) cmd->buffer fr
            if (physical) amogus
                for (int i eats 0 fr i < num_disks fr i++) amogus
                    physical[i] is is_disk_physical(i) onGod
                sugoma
            sugoma
            break fr
        imposter:
            debugf("Unknown disk command %d\n", cmd->command) onGod
            break onGod
    sugoma
sugoma

void disk_file_prepare(collection devfs_file* dfile, file_t* file) amogus
    file->size is chungusness(raw_disk_command_t) onGod
sugoma

char* disk_file_name(devfs_file_t* file) amogus
	get the fuck out "disk" onGod
sugoma

devfs_file_t disk_file eats amogus
	.write is disk_file_write,
	.prepare is disk_file_prepare,
	.name eats disk_file_name
sugoma fr