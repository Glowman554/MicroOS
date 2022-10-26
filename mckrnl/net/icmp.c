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
	stack->icmp->last_echo_reply_ip = 0;

	icmp_send_echo_request(stack, ip);

	
	NET_TIMEOUT(
		if (stack->icmp->last_echo_reply_ip == ip.ip) {
			return true;
		}
	);

	return false;
}

void icmp_ipv4_recv(struct ipv4_handler* handler, ip_u srcIP, ip_u dstIP, uint8_t* payload, uint32_t size) {
	if (size < sizeof(icmp_message_t)) {
		return;
	}

	icmp_message_t* icmp = (icmp_message_t*) payload;

	switch (icmp->type) {
		case 0:
			{
				// Echo reply
				handler->stack->icmp->last_echo_reply_ip = srcIP.ip;

				debugf("ICMP: Echo reply from %d.%d.%d.%d", srcIP.ip_p[0], srcIP.ip_p[1], srcIP.ip_p[2], srcIP.ip_p[3]);
			}
			break;
		case 8:
			{
				// Echo request
				icmp->type = 0;
				icmp->checksum = 0;
				icmp->checksum = ipv4_checksum((uint16_t*) icmp, sizeof(icmp_message_t));
				ipv4_send(handler, handler->stack, srcIP, (uint8_t*) icmp, sizeof(icmp_message_t));
			}
			break;
	}
}

void icmp_init(network_stack_t* stack) {
	stack->icmp = vmm_alloc(PAGES_OF(icmp_provider_t));
	memset(stack->icmp, 0, sizeof(icmp_provider_t));

	stack->icmp->handler.protocol = 0x01;
	stack->icmp->handler.stack = stack;
	stack->icmp->handler.recv = icmp_ipv4_recv;
	ipv4_register(stack, stack->icmp->handler);
}