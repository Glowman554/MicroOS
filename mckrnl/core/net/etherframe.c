#include <net/etherframe.h>
#include <stdio.h>
#include <memory/heap.h>
#include <string.h>
#include <net/stack.h>
#include <config.h>
#ifdef NETWORK_STACK

void etherframe_register(network_stack_t* stack, ether_frame_handler_t handler) {
	debugf("Registering etherframe handler for %d", handler.ether_type_be);
	stack->ether_frame->handlers = krealloc(stack->ether_frame->handlers, sizeof(ether_frame_handler_t) * (stack->ether_frame->num_handlers + 1));
	stack->ether_frame->handlers[stack->ether_frame->num_handlers] = handler;
	stack->ether_frame->num_handlers++;
}

void etherframe_send(ether_frame_handler_t* handler, network_stack_t* stack, uint64_t dest_mac_be, uint8_t* payload, uint32_t size) {
	uint8_t* buffer = kmalloc(sizeof(ether_frame_header_t) + size);

	ether_frame_header_t* frame = (ether_frame_header_t*) buffer;

	frame->dest_mac_be = dest_mac_be;
	frame->src_mac_be = stack->driver->mac.mac;
	frame->ether_type_be = handler->ether_type_be;

	memcpy(buffer + sizeof(ether_frame_header_t), payload, size);
	send_packet(stack->driver, buffer, size + sizeof(ether_frame_header_t));

	kfree(buffer);
}

void etherframe_nic_recv(struct nic_driver* driver, uint8_t* data, uint32_t len) {
	ether_frame_header_t* frame = (ether_frame_header_t*) data;
	mac_u src_mac = { .mac = frame->src_mac_be };

	network_stack_t* stack = driver->driver.driver_specific_data;

	if (frame->dest_mac_be == 0xFFFFFFFFFFFF || frame->dest_mac_be == driver->mac.mac) {
		bool handled = false;
		for (int i = 0; i < stack->ether_frame->num_handlers; i++) {
			if (stack->ether_frame->handlers[i].ether_type_be == frame->ether_type_be) {
				stack->ether_frame->handlers[i].recv(&stack->ether_frame->handlers[i], src_mac, data + sizeof(ether_frame_header_t), len - sizeof(ether_frame_header_t));
				handled = true;
			}
		}

		if (!handled) {
			debugf("--- WARNING --- Unhandled etherframe %x!", frame->ether_type_be);
		}
	}
}

void etherframe_init(network_stack_t* stack) {
	stack->ether_frame = kmalloc(sizeof(ether_frame_provider_t));
	memset(stack->ether_frame, 0, sizeof(ether_frame_provider_t));

	stack->driver->recv = etherframe_nic_recv;
}
#endif