#include <net/etherframe.h>
#include <net/arp.h>
#include <net/ipv4.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <stdio.h>
#include <memory/heap.h>
#include <net/stack.h>
#include <config.h>
#ifdef NETWORK_STACK

bool ethdump_dump_enabled = false;

typedef struct dns_header {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} __attribute__((packed)) dns_header_t;

typedef struct dhcp_message {
    uint8_t op;
    uint8_t hardware_type;
    uint8_t hardware_addr_len;
    uint8_t hops;
    uint32_t xid;
    uint16_t seconds;
    uint16_t flags;
    uint32_t client_ip;
    uint32_t your_ip;
    uint32_t server_ip;
    uint32_t gateway_ip;
    uint8_t client_hardware_addr[16];
} __attribute__((packed)) dhcp_message_t;

void etherframe_dump_mac(const char* label, mac_u mac) {
    debugf(WARNING, "%s %x:%x:%x:%x:%x:%x",
        label,
        mac.mac_p[0], mac.mac_p[1], mac.mac_p[2],
        mac.mac_p[3], mac.mac_p[4], mac.mac_p[5]);
}

const char* tcp_flags_str(uint8_t flags) {
    static char buffer[64];
    int pos = 0;
    buffer[0] = '\0';

    if (flags & SYN) {
        pos += sprintf(buffer + pos, "SYN|");
    }
    if (flags & ACK) {
        pos += sprintf(buffer + pos, "ACK|");
    }
    if (flags & FIN) {
        pos += sprintf(buffer + pos, "FIN|");
    }
    if (flags & RST) {
        pos += sprintf(buffer + pos, "RST|");
    }
    if (flags & PSH) {
        pos += sprintf(buffer + pos, "PSH|");
    }
    if (flags & URG) {
        pos += sprintf(buffer + pos, "URG|");
    }
    if (flags & ECE) {
        pos += sprintf(buffer + pos, "ECE|");
    }

    if (flags & CWR) {
        pos += sprintf(buffer + pos, "CWR|");
    }

    if (buffer[0] == '\0') {
        return "NONE";
    }
    
    buffer[pos - 1] = '\0';
    
    return buffer;
}

void etherframe_dump_dns(uint8_t* payload, uint32_t size) {
    if (size < sizeof(dns_header_t)) {
        return;
    }

    dns_header_t* dns = (dns_header_t*) payload;
    debugf(WARNING, "DNS id=0x%x flags=0x%x qd=%d an=%d ns=%d ar=%d",
        BSWAP16(dns->id), BSWAP16(dns->flags),
        BSWAP16(dns->qdcount), BSWAP16(dns->ancount),
        BSWAP16(dns->nscount), BSWAP16(dns->arcount));
}

void etherframe_dump_dhcp(uint8_t* payload, uint32_t size) {
    if (size < sizeof(dhcp_message_t)) {
        return;
    }

    dhcp_message_t* dhcp = (dhcp_message_t*) payload;
    ip_u client_ip = { .ip = dhcp->client_ip };
    ip_u your_ip = { .ip = dhcp->your_ip };
    ip_u server_ip = { .ip = dhcp->server_ip };
    ip_u gateway_ip = { .ip = dhcp->gateway_ip };

    debugf(WARNING, "DHCP op=%d htype=%d hlen=%d hops=%d xid=0x%x ci=%d.%d.%d.%d yi=%d.%d.%d.%d si=%d.%d.%d.%d gi=%d.%d.%d.%d",
        dhcp->op,
        dhcp->hardware_type,
        dhcp->hardware_addr_len,
        dhcp->hops,
        BSWAP32(dhcp->xid),
        client_ip.ip_p[0], client_ip.ip_p[1], client_ip.ip_p[2], client_ip.ip_p[3],
        your_ip.ip_p[0], your_ip.ip_p[1], your_ip.ip_p[2], your_ip.ip_p[3],
        server_ip.ip_p[0], server_ip.ip_p[1], server_ip.ip_p[2], server_ip.ip_p[3],
        gateway_ip.ip_p[0], gateway_ip.ip_p[1], gateway_ip.ip_p[2], gateway_ip.ip_p[3]);
}

void etherframe_dump_udp_payload(uint8_t* payload, uint32_t size, uint16_t src_port, uint16_t dst_port) {
    if (src_port == 53 || dst_port == 53) {
        etherframe_dump_dns(payload, size);
    }
    if (src_port == 67 || dst_port == 67 || src_port == 68 || dst_port == 68) {
        etherframe_dump_dhcp(payload, size);
    }
}

void etherframe_dump_ipv4(uint8_t* payload, uint32_t size) {
    if (size < sizeof(ipv4_message_t)) {
        debugf(WARNING, "IPv4: packet too small (%u bytes)", size);
        return;
    }

    ipv4_message_t* ipv4 = (ipv4_message_t*) payload;
    uint32_t header_bytes = (uint32_t) ipv4->header_length * 4;
    uint16_t total_length = BSWAP16(ipv4->total_length);
    ip_u source_ip = { .ip = ipv4->source_address };
    ip_u dest_ip = { .ip = ipv4->destination_address };

    debugf(WARNING, "IPv4: %d.%d.%d.%d -> %d.%d.%d.%d proto=%d ttl=%d len=%d hdr=%d",
        source_ip.ip_p[0], source_ip.ip_p[1], source_ip.ip_p[2], source_ip.ip_p[3],
        dest_ip.ip_p[0], dest_ip.ip_p[1], dest_ip.ip_p[2], dest_ip.ip_p[3],
        ipv4->protocol,
        ipv4->time_to_live,
        total_length,
        header_bytes);

    if (size < header_bytes) {
        return;
    }

    uint8_t* l4_payload = payload + header_bytes;
    uint32_t l4_size = size - header_bytes;

    if (ipv4->protocol == 17) {
        if (l4_size < sizeof(udp_header_t)) {
            debugf(WARNING, "UDP: packet too small (%u bytes)", l4_size);
            return;
        }
        udp_header_t* udp = (udp_header_t*) l4_payload;
        uint16_t src_port = BSWAP16(udp->src_port);
        uint16_t dst_port = BSWAP16(udp->dst_port);
        uint16_t udp_length = BSWAP16(udp->length);

        debugf(WARNING, "UDP: %d -> %d length=%d checksum=0x%x",
            src_port, dst_port, udp_length, BSWAP16(udp->checksum));
        etherframe_dump_udp_payload(l4_payload + sizeof(udp_header_t), l4_size - sizeof(udp_header_t), src_port, dst_port);
    } else if (ipv4->protocol == 6) {
        if (l4_size < sizeof(tcp_header_t)) {
            debugf(WARNING, "TCP: packet too small (%u bytes)", l4_size);
            return;
        }
        tcp_header_t* tcp = (tcp_header_t*) l4_payload;
        uint16_t src_port = BSWAP16(tcp->src_port);
        uint16_t dst_port = BSWAP16(tcp->dst_port);
        uint32_t tcp_header_size = (uint32_t) tcp->header_size_32 * 4;

        debugf(WARNING, "TCP: %d -> %d seq=0x%x ack=0x%x flags=%s window=%d hdr=%d",
            src_port,
            dst_port,
            BSWAP32(tcp->sequence_number),
            BSWAP32(tcp->ack_number),
            tcp_flags_str(tcp->flags),
            BSWAP16(tcp->window_size),
            tcp_header_size);
    } else if (ipv4->protocol == 1) {
        if (l4_size >= 2) {
            debugf(WARNING, "ICMP: type=%d code=%d", l4_payload[0], l4_payload[1]);
        }
    }
}

void etherframe_dump_arp(uint8_t* payload, uint32_t size) {
    if (size < sizeof(arp_message_t)) {
        debugf(WARNING, "ARP: packet too small (%u bytes)", size);
        return;
    }

    arp_message_t* arp = (arp_message_t*) payload;
    mac_u source = { .mac = arp->src_mac };
    mac_u target = { .mac = arp->dest_mac };
    ip_u source_ip = { .ip = arp->src_ip };
    ip_u target_ip = { .ip = arp->dest_ip };

    debugf(WARNING, "ARP: command=%d hw=%d proto=%d hlen=%d plen=%d src=%d.%d.%d.%d dst=%d.%d.%d.%d",
        BSWAP16(arp->command),
        BSWAP16(arp->hardware_type),
        BSWAP16(arp->protocol),
        arp->hardware_address_size,
        arp->protocol_address_size,
        source_ip.ip_p[0], source_ip.ip_p[1], source_ip.ip_p[2], source_ip.ip_p[3],
        target_ip.ip_p[0], target_ip.ip_p[1], target_ip.ip_p[2], target_ip.ip_p[3]);
    etherframe_dump_mac("ARP src:", source);
    etherframe_dump_mac("ARP dst:", target);
}

void etherframe_dump(uint8_t* data, uint32_t len) {
    if (!ethdump_dump_enabled) {
        return;
    }

    if (len < sizeof(ether_frame_header_t)) {
        debugf(WARNING, "Etherframe: packet too small (%u bytes)", len);
        return;
    }

	debugf(WARNING, "--- Etherframe Dump ---");

    ether_frame_header_t* frame = (ether_frame_header_t*) data;
    mac_u src_mac = { .mac = frame->src_mac_be };
    mac_u dst_mac = { .mac = frame->dest_mac_be };
    uint16_t ether_type = BSWAP16(frame->ether_type_be);

    debugf(WARNING, "Etherframe: %x:%x:%x:%x:%x:%x -> %x:%x:%x:%x:%x:%x type=0x%x len=%d",
        src_mac.mac_p[0], src_mac.mac_p[1], src_mac.mac_p[2], src_mac.mac_p[3], src_mac.mac_p[4], src_mac.mac_p[5],
        dst_mac.mac_p[0], dst_mac.mac_p[1], dst_mac.mac_p[2], dst_mac.mac_p[3], dst_mac.mac_p[4], dst_mac.mac_p[5],
        ether_type,
        len);

    uint8_t* payload = data + sizeof(ether_frame_header_t);
    uint32_t payload_len = len - sizeof(ether_frame_header_t);

    if (ether_type == 0x0800) {
        etherframe_dump_ipv4(payload, payload_len);
    } else if (ether_type == 0x0806) {
        etherframe_dump_arp(payload, payload_len);
    }

	debugf(WARNING, "--- Etherframe End  ---");
}

#endif