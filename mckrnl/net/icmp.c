#include <net/icmp.h>

#include <memory/vmm.h>
#include <string.h>
#include <stdio.h>
#include <config.h>
#include <scheduler/async.h>
#ifdef NETWORK_STACK

mac_u sync_route_resolve(network_stack_t* stack, ip_u ip) {
	note("sync_route_resolve() called. This function blocks the whole system until the route is resolved and SHOULD NOT be used!");

	async_t async = { .state = STATE_INIT };
	
	mac_u route;
	while (!is_resolved(&async)) {
		route = ipv4_resolve_route(stack, &async, ip);
	}

	return route;
}



void icmp_send_echo_request(network_stack_t* stack, ip_u ip, mac_u route) {
	icmp_message_t icmp = {
		.type = 8,
		.code = 0,
		.checksum = 0,
		.data = 0x3713
	};

	icmp.checksum = ipv4_checksum((uint16_t*) &icmp, sizeof(icmp_message_t));
	ipv4_send(&stack->icmp->handler, stack, ip, route, (uint8_t*) &icmp, sizeof(icmp_message_t));
}

void icmp_send_echo_request_and_wait(network_stack_t* stack, async_t* async, ip_u ip, mac_u route) {
	switch (async->state) {
		case STATE_REQUEST:
			stack->icmp->last_echo_reply_ip = 0;
			icmp_send_echo_request(stack, ip, route);
			async->state = STATE_WAIT;
			break;

		case STATE_WAIT:
			if (stack->icmp->last_echo_reply_ip == ip.ip) {
				async->state = STATE_DONE;
			}
			break;

		case STATE_DONE:
			break;

		default:
			async->state = STATE_REQUEST;
			break;
	}
}

void icmp_async_answer(async_t* async) {
	icmp_async_answer_t* answer = (icmp_async_answer_t*) async->data;

	mac_u route = ipv4_resolve_route(answer->handler->stack, async, answer->srcIP);
	if (is_resolved(async)) {
		answer->icmp.type = 0;
		answer->icmp.checksum = 0;
		answer->icmp.checksum = ipv4_checksum((uint16_t*) &answer->icmp, sizeof(icmp_message_t));

		ipv4_send(answer->handler, answer->handler->stack, answer->srcIP, route, (uint8_t*) &answer->icmp, sizeof(icmp_message_t));
	}
}


void icmp_ipv4_recv(struct ipv4_handler* handler, ip_u srcIP, ip_u dstIP, uint8_t* payload, uint32_t size) {
	if (size < sizeof(icmp_message_t)) {
		return;
	}

	icmp_message_t* icmp = (icmp_message_t*) payload;
	debugf("ICMP: Received ICMP message of type %d from %d.%d.%d.%d", icmp->type, srcIP.ip_p[0], srcIP.ip_p[1], srcIP.ip_p[2], srcIP.ip_p[3]);

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
				icmp_async_answer_t* answer = vmm_alloc(TO_PAGES(sizeof(icmp_async_answer_t)));
				answer->icmp = *icmp;
				answer->handler = handler;
				answer->srcIP = srcIP;

				async_t async = {
					.state = STATE_INIT,
					.data = answer
				};

				add_async_task(icmp_async_answer, async, false);
			}
			break;
		case 3:
			{
				// Destination unreachable message
				debugf("ICMP ---WARNING ---: Destination unreachable (%s)", icmp_destination_unreachable_to_str(icmp->code));
			}
			break;
		default:
			{
				abortf("ICMP type %d not implemented!", icmp->type);
			}
			break;
	}
}

char* icmp_destination_unreachable_to_str(uint8_t code) {
	switch (code) {
		case 0:	return "Network unreachable error.";
		case 1:	return "Host unreachable error.";
		case 2: return "Protocol unreachable error (the designated transport protocol is not supported).";
		case 3: return "Port unreachable error (the designated protocol is unable to inform the host of the incoming message).";
		case 4: return "The datagram is too big. Packet fragmentation is required but the 'don't fragment' (DF) flag is on.";
		case 5:	return "Source route failed error.";
		case 6: return "Destination network unknown error.";
		case 7: return "Destination host unknown error.";
		case 8:	return "Source host isolated error.";
		case 9:	return "The destination network is administratively prohibited.";
		case 10: return "The destination host is administratively prohibited.";
		case 11: return "The network is unreachable for Type Of Service.";
		case 12: return "The host is unreachable for Type Of Service.";
		case 13: return "Communication administratively prohibited (administrative filtering prevents packet from being forwarded).";
		case 14: return "Host precedence violation (indicates the requested precedence is not permitted for the combination of host or network and port).";
		case 15: return "Precedence cutoff in effect (precedence of datagram is below the level set by the network administrators).";
		default: return "unk";
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
#endif