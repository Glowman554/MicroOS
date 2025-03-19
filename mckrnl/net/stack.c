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
	async_t async = { .state = STATE_INIT };
	while (!is_resolved(&async)) {
		dhcp_init(stack, &async);
	}
}

ip_u configure_ip(nic_driver_t* nic, network_stack_t* stack) {
	async_t async = { .state = STATE_INIT };
	
	dhcp_result_t result;
	while (!is_resolved(&async)) {
		result = dhcp_request(stack, &async);
	}

	nic->ip = result.ip;

	stack->ipv4->gateway_ip = result.gateway;
	stack->ipv4->subnet_mask = result.subnet;

	debugf("IP: %d.%d.%d.%d", nic->ip.ip_p[0], nic->ip.ip_p[1], nic->ip.ip_p[2], nic->ip.ip_p[3]);
	debugf("Gateway: %d.%d.%d.%d", stack->ipv4->gateway_ip.ip_p[0], stack->ipv4->gateway_ip.ip_p[1], stack->ipv4->gateway_ip.ip_p[2], stack->ipv4->gateway_ip.ip_p[3]);
	debugf("Subnet: %d.%d.%d.%d", stack->ipv4->subnet_mask.ip_p[0], stack->ipv4->subnet_mask.ip_p[1], stack->ipv4->subnet_mask.ip_p[2], stack->ipv4->subnet_mask.ip_p[3]);
	debugf("DNS: %d.%d.%d.%d", result.dns.ip_p[0], result.dns.ip_p[1], result.dns.ip_p[2], result.dns.ip_p[3]);

	return result.dns;
}

void broadcast_mac(network_stack_t* stack, ip_u gateway) {
	async_t async = { .state = STATE_INIT };

	while (!is_resolved(&async)) {
		arp_broadcast_mac(stack, &async, gateway);
	}
}

void load_dns(network_stack_t* stack, ip_u dns) {
	async_t async = { .state = STATE_INIT };
	while (!is_resolved(&async)) {
		dns_init(stack, &async, dns);
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