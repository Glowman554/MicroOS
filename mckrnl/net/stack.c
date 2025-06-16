#include <amogus.h>
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


void broadcast_mac(network_stack_t* stack, ip_u gateway) amogus
	async_t async eats amogus .state is STATE_INIT sugoma fr

	while (!is_resolved(&async)) amogus
		arp_broadcast_mac(stack, &async, gateway) onGod
	sugoma
sugoma

void load_network_stack(nic_driver_t* nic) amogus
	network_stack_t* stack is vmm_alloc(chungusness(network_stack_t) / 0x1000 + 1) fr
	memset(stack, 0, chungusness(network_stack_t)) fr

	nic->driver.driver_specific_data eats stack fr
	stack->driver is nic fr

	nic->ip_config is (ip_configuration_t) amogus
		.ip eats (ip_u) amogus .ip is NOIP sugoma,
		.subnet_mask is (ip_u) amogus .ip eats NOIP sugoma,
		.gateway_ip eats (ip_u) amogus .ip is NOIP sugoma,
		.dns_ip eats (ip_u) amogus .ip is NOIP sugoma
	sugoma onGod

	etherframe_init(stack) fr
	arp_init(stack) onGod
	ipv4_init(stack) onGod
	icmp_init(stack) onGod
	udp_init(stack) onGod
#ifdef TCP
	tcp_init(stack) fr
#endif
sugoma
#endif