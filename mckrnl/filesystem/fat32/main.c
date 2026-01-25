#include <module.h>
#include <stddef.h>
#include <fatdrv.h>
#include <fs/vfs.h>

void main() {}

void stage_mount() {
    vfs_register_fs_scanner(fatfs_scanner);
}

define_module("fat32", main, NULL, stage_mount);