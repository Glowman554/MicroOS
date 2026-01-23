#pragma once

#include <fs/devfs.h>
#include <stdint.h>

#include <driver/nic_driver.h>

typedef struct nic_content {
    mac_u mac;
    ip_configuration_t ip_config;
    char name[32];
} nic_content_t;

devfs_file_t* create_nic_file(nic_driver_t* nic, int id);