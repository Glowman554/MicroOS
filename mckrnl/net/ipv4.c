#include <net/ipv4.h>
#include <stdio.h>
#include <memory/vmm.h>
#include <string.h>

void ipv4_register(ipv4_provider_t* provider, ipv4_handler_t handler) {
	for (int i = 0; i < MAX_IPV4_HANDLERS; i++) {
        if (provider->handlers[i].recv == 0) {
            debugf("registering ipv4 handler at %d for 0x%x!", i, handler.protocol);
            provider->handlers[i] = handler;
            return;
        }
    }

    abortf("no more handler slots free!");
}

void ipv4_send(ipv4_handler_t* handler, ipv4_provider_t* provider, arp_provider_t* arp_provider, struct nic_driver* driver, ip_u dest_ip, uint8_t* payload, uint32_t size) {
	uint8_t* buffer = vmm_alloc((size + sizeof(ipv4_message_t)) / 0x1000 + 1);

	ipv4_message_t* ipv4 = (ipv4_message_t*) buffer;

	ipv4->version = 4;
	ipv4->header_length = sizeof(ipv4_message_t) / 4;
	ipv4->type_of_service = 0;
	ipv4->total_length = size + sizeof(ipv4_message_t);
	ipv4->total_length = ((ipv4->total_length & 0xFF00) >> 8) | ((ipv4->total_length & 0x00FF) << 8);
	//ipv4->identification = 0x0100;
	//ipv4->flags_and_fragment_offset = 0x0040;
	
	ipv4->identification = 0x0000;
	ipv4->flags_and_fragment_offset = 0x0000;

	ipv4->time_to_live = 0x40;
	ipv4->protocol = handler->protocol;
	ipv4->destination_address = dest_ip.ip;
	ipv4->source_address = driver->ip.ip;

	ipv4->header_checksum = 0;
	ipv4->header_checksum = ipv4_checksum((uint16_t*) ipv4, sizeof(ipv4_message_t));

	memcpy(buffer + sizeof(ipv4_message_t), payload, size);

	ip_u route = dest_ip;
	if((dest_ip.ip & provider->subnet_mask.ip) != (ipv4->source_address & provider->subnet_mask.ip)) {
		route = provider->gateway_ip;
	}

	etherframe_send(&provider->handler, driver, arp_resolve(arp_provider, driver, route).mac, buffer, size + sizeof(ipv4_message_t));

	vmm_free(buffer, (size + sizeof(ipv4_message_t)) / 0x1000 + 1);
}

void ipv4_etherframe_recv(struct ether_frame_handler* handler, uint8_t* payload, uint32_t size) {
	todo();
}

uint16_t ipv4_checksum(uint16_t* data, uint32_t size) {
	uint32_t temp = 0;

	for(int i = 0; i < size/2; i++) {
		temp += ((data[i] & 0xFF00) >> 8) | ((data[i] & 0x00FF) << 8);
	}

	if(size % 2) {
		temp += ((uint16_t)((char*)data)[size-1]) << 8;
	}

	while(temp & 0xFFFF0000) {
		temp = (temp & 0xFFFF) + (temp >> 16);
	}

	return ((~temp & 0xFF00) >> 8) | ((~temp & 0x00FF) << 8);
}