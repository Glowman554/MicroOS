#include <net/stack.h>
#include <memory/vmm.h>
#include <string.h>

#include <net/etherframe.h>
#include <net/arp.h>
#include <net/ipv4.h>
#include <net/icmp.h>
#include <net/udp.h>
#include <net/dhcp.h>
#include <net/ntp.h>
#include <net/dns.h>

#include <stdio.h>

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
	dhcp_init(stack);

	dhcp_request(stack);

	nic->ip = stack->dhcp->ip;
	stack->ipv4->gateway_ip = stack->dhcp->gateway;
	stack->ipv4->subnet_mask = stack->dhcp->subnet;

	arp_broadcast_mac(stack, stack->dhcp->gateway);

	dns_init(stack, stack->dhcp->dns);
	ntp_init(stack, dns_resolve_A(stack, TIMESERVER));

	time_t t = ntp_time(stack);
	char out[0xff] = { 0 };
	time_format(out, &t);
	debugf("time: %s", out);
}
