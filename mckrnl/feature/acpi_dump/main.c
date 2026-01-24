#include <module.h>
#include <stddef.h>
#include <acpi_vfs.h>

#include <driver/acpi/rsdp.h>
#include <stdio.h>

void main() {
}

void stage_mount() {
    vfs_mount(acpi_vfs_mount());
}

define_module("acpi_dump", main, NULL, stage_mount);