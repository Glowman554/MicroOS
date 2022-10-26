#include <net/icmp.h>

#include <memory/vmm.h>
#include <string.h>
#include <stdio.h>

void icmp_send_echo_request(network_stack_t* stack, ip_u ip) {
	icmp_message_t icmp = {
		.type = 8,
		.code = 0,
		.checksum = 0,
		.data = 0x3713
	};

	icmp.checksum = ipv4_checksum((uint16_t*) &icmp, sizeof(icmp_message_t));
	ipv4_send(&stack->icmp->handler, stack, ip, (uint8_t*) &icmp, sizeof(icmp_message_t));
}

bool icmp_send_echo_reqest_and_wait(network_stack_t* stack, ip_u ip) {
	icmp_send_echo_request(stack, ip);

	stack->icmp->last_echo_reply_ip = 0;

	int timeout = 10000000;
	note("this isnt the best way to implement a timeout. FIXME!");

	while(timeout--) {
		if (stack->icmp->last_echo_reply_ip == ip.ip) {
			return true;
		}
	}

	return false;
}

void icmp_ipv4_recv(struct ipv4_handler* handler, ip_u srcIP, ip_u dstIP, uint8_t* payload, uint32_t size) {
	todo();
}

void icmp_init(network_stack_t* stack) {
	stack->icmp = vmm_alloc(PAGES_OF(icmp_provider_t));
	memset(stack->icmp, 0, sizeof(icmp_provider_t));

	stack->icmp->handler.protocol = 0x01;
	stack->icmp->handler.stack = stack;
	stack->icmp->handler.recv = icmp_ipv4_recv;
	ipv4_register(stack, stack->icmp->handler);
}