#include <driver/network/vlan.h>
#include <memory/vmm.h>
#include <stdint.h>
#include <string.h>
#include <net/stack.h>

#define isdigit(c) (c >= '0' && c <= '9')

#ifdef NETWORK_STACK

bool vlan_is_device_present(driver_t* driver) {
    return true;
}

char* vlan_get_device_name(driver_t* driver) {
    vlan_driver_t* vlan = (vlan_driver_t*) driver;
    return vlan->name;
}

void vlan_init(driver_t* driver) {
    vlan_driver_t* vlan = (vlan_driver_t*) driver;

    vlan->driver.mac = get_nic_driver(vlan->parent)->mac;

    register_nic_driver(&vlan->driver);
}

void vlan_send(nic_driver_t* driver, async_t* async, uint8_t* data, uint32_t size) {
    vlan_driver_t* vlan = (vlan_driver_t*) driver;

    switch (async->state) {
        case STATE_INIT:
        {
            uint8_t* newData = vmm_alloc(TO_PAGES(size + 4));

            ether_frame_header_t* oldFrame = (ether_frame_header_t*) data;
            ether_frame_header_vlan_t* newFrame = (ether_frame_header_vlan_t*) newData;

            memcpy(newData, data, 12);

            newFrame->dest_mac_be = oldFrame->dest_mac_be;
            newFrame->src_mac_be = oldFrame->src_mac_be;
            newFrame->ether_type_be = oldFrame->ether_type_be;
            newFrame->tpid_be = BSWAP16(0x8100);
            newFrame->tci_be = BSWAP16(vlan->vlanid & 0x0FFF);

            memcpy(newData + 16, data + 12, size - 12);

            nic_driver_t* p = get_nic_driver(vlan->parent);
            send_packet(p, newData, size + 4);

            vmm_free(newData, TO_PAGES(size + 4));
            async->state = STATE_DONE;
        }
        break;
    }

 
}

void vlan_etherframe_recv(struct ether_frame_handler* handler, mac_u src_mac, uint8_t* payload, uint32_t size) {
    vlan_driver_t* vlan = (vlan_driver_t*) handler->stack->driver;

    ether_frame_header_vlan_partial_t* partial = (ether_frame_header_vlan_partial_t*) payload;
    // uint16_t vlan_id =  BSWAP16(partial->tci_be) & 0x0FFF;
    // debugf("VLAN ID: %d", vlan_id);

    uint32_t new_size = size - 4 + 14;
    uint8_t* new_frame = vmm_alloc(TO_PAGES(new_size));

    ether_frame_header_t* etherframe = (ether_frame_header_t*) new_frame;
    etherframe->dest_mac_be = vlan->driver.mac.mac;
    etherframe->src_mac_be = src_mac.mac;
    etherframe->ether_type_be = partial->ether_type_be;

    memcpy(new_frame + 14, payload + 4,size - 4);

    vlan->driver.recv(&vlan->driver, new_frame, new_size);

    vmm_free(new_frame, TO_PAGES(new_size));
}

void vlan_stack(nic_driver_t* driver, void* stack) {
    vlan_driver_t* vlan = (vlan_driver_t*) driver;
    network_stack_t* nstack = stack;
    network_stack_t* pstack = get_nic_driver(vlan->parent)->driver.driver_specific_data;

    vlan->handler.ether_type_be = BSWAP16(0x8100);
    vlan->handler.stack = nstack;
    vlan->handler.recv = vlan_etherframe_recv;
    etherframe_register(pstack, vlan->handler);
}



nic_driver_t* get_vlan_driver(int vlanid, int parent) {
	vlan_driver_t* driver = vmm_alloc(PAGES_OF(vlan_driver_t));
	memset(driver, 0, sizeof(vlan_driver_t));

	*driver = (vlan_driver_t) {
		.driver = {
			.driver = {
				.get_device_name = vlan_get_device_name,
				.is_device_present = vlan_is_device_present,
				.init = vlan_init
			},
			.send = vlan_send,
            .stack = vlan_stack
		},
        .vlanid = vlanid,
        .parent = parent,
	};

    sprintf(driver->name, "vlan%d.%d", vlanid, parent);

	return (nic_driver_t*) driver;
}

void configure_vlan(const char* cfg) {
    const char* p = cfg;
    while (*p) {
        int vlan_id = 0;
        while (*p && isdigit((unsigned char)*p)) {
            vlan_id = vlan_id * 10 + (*p - '0');
            p++;
        }

        if (*p != '.') {
            abortf(false, "Invalid VLAN config: expected '.' after VLAN ID\n");
        }
        p++;

        int parent_if = 0;
        while (*p && isdigit((unsigned char)*p)) {
            parent_if = parent_if * 10 + (*p - '0');
            p++;
        }

        if (vlan_id <= 0 || vlan_id > 4095) {
            abortf(false, "Invalid VLAN ID: %d\n", vlan_id);
        }

        debugf("VLAN: vlan%d.%d", vlan_id, parent_if);
        register_driver((driver_t*) get_vlan_driver(vlan_id, parent_if));

        if (*p == ',') {
            p++;
        } else if (*p && !isdigit((unsigned char)*p)) {
            abortf(false, "Invalid VLAN config: unexpected char '%c'\n", *p);
        }
    }
}
#endif