#include "driver/driver.h"
#include <net/stack.h>
#include <memory/vmm.h>
#include <string.h>

#include <net/etherframe.h>
#include <net/arp.h>
#include <net/ipv4.h>
#include <net/icmp.h>
#include <net/udp.h>
#include <net/tcp.h>

#include <config.h>
#ifdef NETWORK_STACK


void broadcast_mac(network_stack_t* stack, ip_u gateway) {
	async_t async = { .state = STATE_INIT };

	while (!is_resolved(&async)) {
		arp_broadcast_mac(stack, &async, gateway);
	}
}

void load_network_stack(nic_driver_t* nic) {
	network_stack_t* stack = vmm_alloc(sizeof(network_stack_t) / 0x1000 + 1);
	memset(stack, 0, sizeof(network_stack_t));

	nic->driver.driver_specific_data = stack;
	stack->driver = nic;

	nic->ip_config = (ip_configuration_t) {
		.ip = (ip_u) { .ip = NOIP },
		.subnet_mask = (ip_u) { .ip = NOIP },
		.gateway_ip = (ip_u) { .ip = NOIP },
		.dns_ip = (ip_u) { .ip = NOIP }
	};

	if (strcmp(nic->driver.get_device_name((driver_t*) nic), "loopback") == 0) {
		nic->ip_config = (ip_configuration_t) {
			.ip = (ip_u) { .ip = 0x0100007f },
			.subnet_mask = (ip_u) { .ip = 0x000000ff },
			.gateway_ip = (ip_u) { .ip = NOIP },
			.dns_ip = (ip_u) { .ip = NOIP }
		};
	}

	etherframe_init(stack);
	arp_init(stack);
	ipv4_init(stack);
	icmp_init(stack);
	udp_init(stack);
#ifdef TCP
	tcp_init(stack);
#endif

	stack->driver->stack(stack->driver, stack);
}
#endif