#pragma once

#include <nettools.h>


typedef struct nic_content {
    mac_u mac;
    ip_configuration_t ip_config;
} nic_content_t;

nic_content_t nic_read(int nic);
void nic_write(int nic, nic_content_t content);

mac_u sync_ipv4_resolve_route(int nic, ip_u dest_ip);

