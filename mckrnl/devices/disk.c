#include <devices/disk.h>
#include <driver/disk_driver.h>
#include <stdio.h>
#include <assert.h>

void disk_file_write(struct devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) {
    assert(sizeof(raw_disk_command_t) == size && offset == 0);

    raw_disk_command_t* cmd = (raw_disk_command_t*) buf;

    switch (cmd->command) {
        case 1:
            read_disk(cmd->disk, cmd->sector, cmd->sector_count, cmd->buffer);
            break;
        case 2:
            write_disk(cmd->disk, cmd->sector, cmd->sector_count, cmd->buffer);
            break;
        case 3:
            cmd->disk = num_disks;
            bool* physical = (bool*) cmd->buffer;
            if (physical) {
                for (int i = 0; i < num_disks; i++) {
                    physical[i] = is_disk_physical(i);
                }
            }
            break;
        default:
            debugf("Unknown disk command %d\n", cmd->command);
            break;
    }
}

void disk_file_prepare(struct devfs_file* dfile, file_t* file) {
}

char* disk_file_name(devfs_file_t* file) {
	return "disk";
}

devfs_file_t disk_file = {
	.write = disk_file_write,
	.prepare = disk_file_prepare,
	.name = disk_file_name
};