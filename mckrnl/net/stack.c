#include <net/stack.h>
#include <memory/vmm.h>
#include <string.h>

#include <net/etherframe.h>
#include <net/arp.h>
#include <net/ipv4.h>
#include <net/icmp.h>

#include <stdio.h>

void load_network_stack(nic_driver_t* nic) {
	ip_u my_ip = {.ip_p = {10, 0, 2, 15}};
	ip_u gateway_ip = {.ip_p = {10, 0, 2, 2}};
	ip_u subnet_mask = {.ip_p = {255, 255, 255, 0}};

	network_stack_t* stack = vmm_alloc(sizeof(network_stack_t) / 0x1000 + 1);
	memset(stack, 0, sizeof(network_stack_t));

	nic->driver.driver_specific_data = stack;
	stack->driver = nic;

	etherframe_init(stack);
	arp_init(stack);
	ipv4_init(stack, gateway_ip, subnet_mask);
	icmp_init(stack);

	nic->ip = my_ip;

	arp_broadcast_mac(stack, gateway_ip);

	bool res = icmp_send_echo_reqest_and_wait(stack, (ip_u) {.ip_p = {10, 0, 2, 2}});
	debugf("icmp: %s", res ? "true" : "false");
}
