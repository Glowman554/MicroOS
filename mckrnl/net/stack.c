#include <net/stack.h>
#include <net/swap.h>
#include <memory/vmm.h>
#include <string.h>

void load_network_stack(nic_driver_t* nic) {
	ip_u my_ip = {.ip_p = {10, 0, 2, 15}};
	ip_u gateway_ip = {.ip_p = {10, 0, 2, 2}};
	ip_u subnet_mask = {.ip_p = {255, 255, 255, 0}};

	network_stack_t* stack = vmm_alloc(sizeof(network_stack_t) / 0x1000 + 1);
	memset(stack, 0, sizeof(network_stack_t));

	nic->driver.driver_specific_data = stack;
	nic->recv = etherframe_nic_recv;
	stack->driver = nic;

	stack->arp.handler.ether_type_be = BSWAP16(0x806);
	stack->arp.handler.data = stack;
	stack->arp.handler.recv = arp_etherframe_recv;
	etherframe_register(&stack->ehter_frame, stack->arp.handler);

	stack->ipv4.handler.ether_type_be = BSWAP16(0x0800);
	stack->ipv4.handler.data = stack;
	stack->arp.handler.recv = ipv4_etherframe_recv;
	etherframe_register(&stack->ehter_frame, stack->ipv4.handler);
	stack->ipv4.gateway_ip = gateway_ip;
	stack->ipv4.subnet_mask = subnet_mask;

	nic->ip = my_ip;

	arp_broadcast_mac(&stack->arp, nic, gateway_ip);

	ipv4_handler_t h = {
		.protocol = 10,
		.recv = 0,
		.data = &stack
	};

	ipv4_send(&h, &stack->ipv4, &stack->arp, nic, (ip_u) {.ip_p = {8, 8, 8, 8}}, "hello world", 11);
}
