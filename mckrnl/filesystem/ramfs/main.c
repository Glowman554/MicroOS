#include <module.h>
#include <stddef.h>
#include <ramfs.h>
#include <fs/vfs.h>

void main() {}

void stage_mount() {
    vfs_mount(get_ramfs("tmp"));
}

define_module("ramfs", main, NULL, stage_mount);