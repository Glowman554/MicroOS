#pragma once

#include <driver/nic_driver.h>

struct ether_frame_provider;
struct arp_provider;
struct ipv4_provider;

typedef struct network_stack {
	struct ether_frame_provider* ether_frame;
	struct arp_provider* arp;
	struct ipv4_provider* ipv4;
	nic_driver_t* driver;
} network_stack_t;

void load_network_stack(nic_driver_t* nic);

// #include <net/etherframe.h>
// #include <net/arp.h>
// #include <net/ipv4.h>


#define BSWAP16(n) ((n & 0x00FF) << 8) | ((n & 0xFF00) >> 8)
#define BSWAP32(n)  ((n & 0xFF000000) >> 24) | ((n & 0x00FF0000) >> 8) | ((n & 0x0000FF00) << 8) | ((n & 0x000000FF) << 24)