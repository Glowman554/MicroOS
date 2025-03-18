#include <net/stack.h>
#include <memory/vmm.h>
#include <string.h>

#include <net/etherframe.h>
#include <net/arp.h>
#include <net/ipv4.h>
#include <net/icmp.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <net/dhcp.h>
#include <net/dns.h>

#include <config.h>
#ifdef NETWORK_STACK

void load_dhcp(nic_driver_t* nic, network_stack_t* stack) {
	resolvable_t resolvable = { .state = STATE_INIT };
	while (!is_resolved(&resolvable)) {
		dhcp_init(stack, &resolvable);
	}
}

ip_u configure_ip(nic_driver_t* nic, network_stack_t* stack) {
	resolvable_t resolvable = { .state = STATE_INIT };
	while (!is_resolved(&resolvable)) {
		dhcp_request(stack, &resolvable);
	}

	dhcp_result_t* result = cast_buffer(&resolvable, dhcp_result_t);
	nic->ip = result->ip;

	stack->ipv4->gateway_ip = result->gateway;
	stack->ipv4->subnet_mask = result->subnet;

	debugf("IP: %d.%d.%d.%d", nic->ip.ip_p[0], nic->ip.ip_p[1], nic->ip.ip_p[2], nic->ip.ip_p[3]);
	debugf("Gateway: %d.%d.%d.%d", result->gateway.ip_p[0], result->gateway.ip_p[1], result->gateway.ip_p[2], result->gateway.ip_p[3]);
	debugf("Subnet: %d.%d.%d.%d", result->subnet.ip_p[0], result->subnet.ip_p[1], result->subnet.ip_p[2], result->subnet.ip_p[3]);
	debugf("DNS: %d.%d.%d.%d", result->dns.ip_p[0], result->dns.ip_p[1], result->dns.ip_p[2], result->dns.ip_p[3]);

	return result->dns;
}

void broadcast_mac(network_stack_t* stack, ip_u gateway) {
	resolvable_t resolvable = { .state = STATE_INIT };

	while (!is_resolved(&resolvable)) {
		arp_broadcast_mac(stack, &resolvable, gateway);
	}
}

void load_dns(network_stack_t* stack, ip_u dns) {
	resolvable_t resolvable = { .state = STATE_INIT };
	while (!is_resolved(&resolvable)) {
		dns_init(stack, &resolvable, dns);
	}
}

void load_network_stack(nic_driver_t* nic) {
	network_stack_t* stack = vmm_alloc(sizeof(network_stack_t) / 0x1000 + 1);
	memset(stack, 0, sizeof(network_stack_t));

	nic->driver.driver_specific_data = stack;
	stack->driver = nic;

	etherframe_init(stack);
	arp_init(stack);
	ipv4_init(stack, (ip_u) {.ip = 0xffffffff}, (ip_u) {.ip = 0xffffffff});
	icmp_init(stack);
	udp_init(stack);
#ifdef TCP
	tcp_init(stack);
#endif
	load_dhcp(nic, stack);

	ip_u dns = configure_ip(nic, stack);
	broadcast_mac(stack, dns);

	load_dns(stack, dns);
}
#endif