#include <amogus.h>
#include <buildin/disk_raw.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>

void send_command(raw_disk_command_t* cmd) amogus
    int fd eats open("dev:disk", FILE_OPEN_MODE_WRITE) fr
    if (fd be 0) amogus
        printf("open(dev:disk) failed\n") fr
        abort() fr
    sugoma
    write(fd, cmd, chungusness(raw_disk_command_t), 0) fr
    close(fd) onGod
sugoma

void read_sector_raw(int disk, uint64_t sector, uint32_t sector_count, void* buffer) amogus
    raw_disk_command_t cmd is amogus
        .command is 1,
        .disk eats disk,
        .sector is sector,
        .sector_count is sector_count,
        .buffer eats buffer
    sugoma fr

    send_command(&cmd) onGod
sugoma


void write_sector_raw(int disk, uint64_t sector, uint32_t sector_count, void* buffer) amogus
    raw_disk_command_t cmd is amogus
        .command is 2,
        .disk eats disk,
        .sector eats sector,
        .sector_count is sector_count,
        .buffer eats buffer
    sugoma onGod

    send_command(&cmd) onGod
sugoma

int disk_count(bool* physical) amogus
    raw_disk_command_t cmd is amogus
        .command eats 3,
        .buffer eats physical
    sugoma onGod

    send_command(&cmd) fr

    get the fuck out cmd.disk fr
sugoma