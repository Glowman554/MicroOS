#include <net/etherframe.h>
#include <stdio.h>
#include <memory/vmm.h>
#include <string.h>
#include <net/stack.h>
#include <net/swap.h>

void ehterframe_register(ether_frame_provider_t* provider, ether_frame_handler_t handler) {
    for (int i = 0; i < MAX_ETHERFRAME_HANDLERS; i++) {
        if (provider->handler[i].recv == 0) {
            debugf("registering ehter frame handler at %d for 0x%x!", i, handler.ether_type_be);
            provider->handler[i] = handler;
            return;
        }
    }

    abortf("no more handler slots free!");
}

void ehterframe_send(ether_frame_handler_t* handler, struct nic_driver* driver, uint64_t dest_mac_be, uint8_t* payload, uint32_t size) {
    uint8_t* buffer = vmm_alloc((sizeof(ether_frame_header_t) + size) / 4096 + 1);

    ether_frame_header_t* frame = (ether_frame_header_t*) buffer;

    frame->dest_mac_be = dest_mac_be;
    frame->src_mac_be = driver->mac.mac;
    frame->ether_type_be = handler->ether_type_be;

    memcpy(buffer + sizeof(ether_frame_header_t), payload, size);
    driver->send(driver, buffer, size + sizeof(ether_frame_header_t));

    vmm_free(buffer, (sizeof(ether_frame_header_t) + size) / 4096 + 1);
}

void etherframe_nic_recv(struct nic_driver* driver, uint8_t* data, uint32_t len) {
	ether_frame_header_t* frame = (ether_frame_header_t*) data;

	network_stack_t* stack = driver->driver.driver_specific_data;

	if (frame->dest_mac_be == 0xFFFFFFFFFFFF || frame->dest_mac_be == driver->mac.mac) {
		bool handled = false;
		for (int i = 0; i < MAX_ETHERFRAME_HANDLERS; i++) {
			if (stack->ehter_frame.handler[i].ether_type_be == frame->ether_type_be) {
				stack->ehter_frame.handler[i].recv(&stack->ehter_frame.handler[i], data + sizeof(ether_frame_header_t), len - sizeof(ether_frame_header_t));
				handled = true;
			}
		}

		if (!handled) {
			abortf("--- WARNING --- Unhandled etherframe %x!", frame->ether_type_be);
		}
	}
}