#include <buildin/disk_raw.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>

void send_command(raw_disk_command_t* cmd) {
    int fd = open("dev:disk", FILE_OPEN_MODE_WRITE);
    if (fd == 0) {
        printf("open(dev:disk) failed\n");
        abort();
    }
    write(fd, cmd, sizeof(raw_disk_command_t), 0);
    close(fd);
}

void read_sector_raw(int disk, uint64_t sector, uint32_t sector_count, void* buffer) {
    raw_disk_command_t cmd = {
        .command = 1,
        .disk = disk,
        .sector = sector,
        .sector_count = sector_count,
        .buffer = buffer
    };

    send_command(&cmd);
}


void write_sector_raw(int disk, uint64_t sector, uint32_t sector_count, void* buffer) {
    raw_disk_command_t cmd = {
        .command = 2,
        .disk = disk,
        .sector = sector,
        .sector_count = sector_count,
        .buffer = buffer
    };

    send_command(&cmd);
}

int disk_count(bool* physical) {
    raw_disk_command_t cmd = {
        .command = 3,
        .buffer = physical
    };

    send_command(&cmd);

    return cmd.disk;
}