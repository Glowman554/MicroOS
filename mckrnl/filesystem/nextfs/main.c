#include <module.h>
#include <stddef.h>
#include <nextfs.h>
#include <fs/vfs.h>

void main() {}

void stage_mount() {
    vfs_register_fs_scanner(nextfs_scanner);
}

define_module("nextfs", main, NULL, stage_mount);