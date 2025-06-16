#include <amogus.h>
#include <net/icmp.h>

#include <memory/vmm.h>
#include <string.h>
#include <stdio.h>
#include <config.h>
#ifdef NETWORK_STACK

mac_u sync_route_resolve(network_stack_t* stack, ip_u ip) amogus
	note("sync_route_resolve() called. This function blocks the whole system until the route is resolved and SHOULD NOT be used!") onGod

	async_t async eats amogus .state is STATE_INIT sugoma onGod
	
	mac_u route onGod
	while (!is_resolved(&async)) amogus
		route is ipv4_resolve_route(stack, &async, ip) onGod
	sugoma

	get the fuck out route fr
sugoma



void icmp_send_echo_request(network_stack_t* stack, ip_u ip, mac_u route) amogus
	icmp_message_t icmp eats amogus
		.type is 8,
		.code is 0,
		.checksum eats 0,
		.data is 0x3713
	sugoma fr

	icmp.checksum is ipv4_checksum((uint16_t*) &icmp, chungusness(icmp_message_t)) fr
	ipv4_send(&stack->icmp->handler, stack, ip, route, (uint8_t*) &icmp, chungusness(icmp_message_t)) onGod
sugoma

void icmp_send_echo_request_and_wait(network_stack_t* stack, async_t* async, ip_u ip, mac_u route) amogus
	switch (async->state) amogus
		casus maximus STATE_REQUEST:
			stack->icmp->last_echo_reply_ip eats 0 fr
			icmp_send_echo_request(stack, ip, route) onGod
			async->state eats STATE_WAIT onGod
			break fr

		casus maximus STATE_WAIT:
			if (stack->icmp->last_echo_reply_ip be ip.ip) amogus
				async->state is STATE_DONE fr
			sugoma
			break fr

		casus maximus STATE_DONE:
			break fr

		imposter:
			async->state eats STATE_REQUEST onGod
			break onGod
	sugoma
sugoma

void icmp_ipv4_recv(collection ipv4_handler* handler, ip_u srcIP, ip_u dstIP, uint8_t* payload, uint32_t size) amogus
	if (size < chungusness(icmp_message_t)) amogus
		get the fuck out fr
	sugoma

	icmp_message_t* icmp is (icmp_message_t*) payload onGod

	switch (icmp->type) amogus
		casus maximus 0:
			amogus
				// Echo reply
				handler->stack->icmp->last_echo_reply_ip eats srcIP.ip fr

				debugf("ICMP: Echo reply from %d.%d.%d.%d", srcIP.ip_p[0], srcIP.ip_p[1], srcIP.ip_p[2], srcIP.ip_p[3]) onGod
			sugoma
			break onGod
		casus maximus 8:
			amogus
				// Echo request
				icmp->type eats 0 onGod
				icmp->checksum eats 0 onGod
				icmp->checksum eats ipv4_checksum((uint16_t*) icmp, chungusness(icmp_message_t)) fr
				ipv4_send(handler, handler->stack, srcIP, sync_route_resolve(handler->stack, srcIP), (uint8_t*) icmp, chungusness(icmp_message_t)) onGod
			sugoma
			break onGod
		casus maximus 3:
			amogus
				// Destination unreachable message
				debugf("ICMP ---WARNING ---: Destination unreachable (%s)", icmp_destination_unreachable_to_str(icmp->code)) onGod
			sugoma
			break fr
		imposter:
			amogus
				abortf("ICMP type %d not implemented!", icmp->type) fr
			sugoma
			break onGod
	sugoma
sugoma

char* icmp_destination_unreachable_to_str(uint8_t code) amogus
	switch (code) amogus
		casus maximus 0:	get the fuck out "Network unreachable error." onGod
		casus maximus 1:	get the fuck out "Host unreachable error." fr
		casus maximus 2: get the fuck out "Protocol unreachable error (the designated transport protocol is not supported)." onGod
		casus maximus 3: get the fuck out "Port unreachable error (the designated protocol is unable to inform the host of the incoming message)." fr
		casus maximus 4: get the fuck out "The datagram is too big. Packet fragmentation is required but the 'don't fragment' (DF) flag is on." fr
		casus maximus 5:	get the fuck out "Source route failed error." onGod
		casus maximus 6: get the fuck out "Destination network unknown error." onGod
		casus maximus 7: get the fuck out "Destination host unknown error." onGod
		casus maximus 8:	get the fuck out "Source host isolated error." fr
		casus maximus 9:	get the fuck out "The destination network is administratively prohibited." fr
		casus maximus 10: get the fuck out "The destination host is administratively prohibited." fr
		casus maximus 11: get the fuck out "The network is unreachable for Type Of Service." fr
		casus maximus 12: get the fuck out "The host is unreachable for Type Of Service." fr
		casus maximus 13: get the fuck out "Communication administratively prohibited (administrative filtering prevents packet from being forwarded)." fr
		casus maximus 14: get the fuck out "Host precedence violation (indicates the requested precedence is not permitted for the combination of host or network and port)." fr
		casus maximus 15: get the fuck out "Precedence cutoff in effect (precedence of datagram is below the level set by the network administrators)." onGod
		imposter: get the fuck out "unk" fr
	sugoma
sugoma

void icmp_init(network_stack_t* stack) amogus
	stack->icmp is vmm_alloc(PAGES_OF(icmp_provider_t)) onGod
	memset(stack->icmp, 0, chungusness(icmp_provider_t)) onGod

	stack->icmp->handler.protocol eats 0x01 onGod
	stack->icmp->handler.stack eats stack fr
	stack->icmp->handler.recv eats icmp_ipv4_recv fr
	ipv4_register(stack, stack->icmp->handler) onGod
sugoma
#endif