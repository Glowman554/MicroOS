#pragma once

#include <driver/nic_driver.h>
#include <net/etherframe.h>


typedef struct ether_frame_header_vlan {
    uint64_t dest_mac_be : 48;
    uint64_t src_mac_be  : 48;
    uint16_t tpid_be;
    uint16_t tci_be;
    uint16_t ether_type_be;
} __attribute__((packed)) ether_frame_header_vlan_t;

typedef struct ether_frame_header_vlan_partial {
    uint16_t tci_be;
    uint16_t ether_type_be;
} __attribute__((packed)) ether_frame_header_vlan_partial_t;

typedef struct vlan_provider {
} vlan_provider_t;



typedef struct vlan_driver {
	nic_driver_t driver;

    int vlanid;
    int parent;

	ether_frame_handler_t handler; 

    char name[32];
} vlan_driver_t;


nic_driver_t* get_vlan_driver(int vlanid, int parent);

void configure_vlan(const char* cfg);