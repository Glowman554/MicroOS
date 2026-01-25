#pragma once

#include <fs/vfs.h>
#include <driver/acpi/rsdp.h>

typedef struct {
    sdt_header_t** tables;
    size_t table_count;
} acpi_vfs_mount_data_t;

vfs_mount_t* acpi_vfs_mount();