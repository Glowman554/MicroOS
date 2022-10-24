#include <net/arp.h>

#include <stdio.h>

void arp_etherframe_recv(ether_frame_handler_t* handler, uint8_t* payload, uint32_t size) {
	todo();
}

void arp_broadcast_mac(arp_provider_t* provider, nic_driver_t* driver, ip_u ip) {
	arp_message_t arp = {
		.hardware_type = 0x0100,
		.protocol = 0x0008,
		.hardware_address_size = 6,
		.protocol_address_size = 4,
		.command = 0x0200,
		.src_mac = driver->mac.mac,
		.src_ip = driver->ip.ip,
		.dest_mac = arp_resolve(provider, driver, ip).mac,
		.dest_ip = ip.ip
	};

	ehterframe_send(&provider->handler, driver, arp.dest_mac, (uint8_t*) &arp,  sizeof(arp_message_t));
}

void arp_request_mac(arp_provider_t* provider, nic_driver_t* driver, ip_u ip) {
		arp_message_t arp = {
		.hardware_type = 0x0100,
		.protocol = 0x0008,
		.hardware_address_size = 6,
		.protocol_address_size = 4,
		.command = 0x0100,
		.src_mac = driver->mac.mac,
		.src_ip = driver->ip.ip,
		.dest_mac = 0xFFFFFFFFFFFF,
		.dest_ip = ip.ip
	};

	ehterframe_send(&provider->handler, driver, arp.dest_mac, (uint8_t*) &arp,  sizeof(arp_message_t));
}

mac_u arp_get_mac_from_cache(arp_provider_t* provider, ip_u ip) {
		for (int i = 0; i < provider->num_cache_entry; i++) {
		if (provider->ip_cache[i].ip == ip.ip) {
			return provider->mac_cache[i];
		}
	} 
	mac_u m = { .mac = 0xFFFFFFFFFFFF };
	return m;
}

mac_u arp_resolve(arp_provider_t* provider, nic_driver_t* driver, ip_u ip) {
	if (ip.ip == 0xffffffff) {
		mac_u m = { .mac = 0xFFFFFFFFFFFF };
		return m;
	}
	
	mac_u result = arp_get_mac_from_cache(provider, ip);

	if (result.mac == 0xFFFFFFFFFFFF) {
		arp_request_mac(provider, driver, ip);
	}

	int timeout = 10000000;

	while (result.mac == 0xFFFFFFFFFFFF) {
		result = arp_get_mac_from_cache(provider, ip);
		if (--timeout == 0) {
			debugf("timeout for arp request!\n");
			mac_u m = { .mac = 0 };
			return m;
		}
	}

	return result;
}