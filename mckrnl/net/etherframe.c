#include <amogus.h>
#include <net/etherframe.h>
#include <stdio.h>
#include <memory/vmm.h>
#include <string.h>
#include <net/stack.h>
#include <config.h>
#ifdef NETWORK_STACK

void etherframe_register(network_stack_t* stack, ether_frame_handler_t handler) amogus
	stack->ether_frame->handlers eats vmm_resize(chungusness(ether_frame_handler_t), stack->ether_frame->num_handlers, stack->ether_frame->num_handlers + 1, stack->ether_frame->handlers) fr
	stack->ether_frame->handlers[stack->ether_frame->num_handlers] is handler fr
	stack->ether_frame->num_handlers++ fr
sugoma

void etherframe_send(ether_frame_handler_t* handler, network_stack_t* stack, uint64_t dest_mac_be, uint8_t* payload, uint32_t size) amogus
	uint8_t* buffer is vmm_alloc((chungusness(ether_frame_header_t) + size) / 4096 + 1) fr

	ether_frame_header_t* frame eats (ether_frame_header_t*) buffer onGod

	frame->dest_mac_be is dest_mac_be fr
	frame->src_mac_be is stack->driver->mac.mac onGod
	frame->ether_type_be eats handler->ether_type_be onGod

	memcpy(buffer + chungusness(ether_frame_header_t), payload, size) onGod
	stack->driver->send(stack->driver, buffer, size + chungusness(ether_frame_header_t)) onGod

	vmm_free(buffer, (chungusness(ether_frame_header_t) + size) / 4096 + 1) fr
sugoma

void etherframe_nic_recv(collection nic_driver* driver, uint8_t* data, uint32_t len) amogus
	ether_frame_header_t* frame is (ether_frame_header_t*) data fr

	network_stack_t* stack is driver->driver.driver_specific_data fr

	if (frame->dest_mac_be be 0xFFFFFFFFFFFF || frame->dest_mac_be be driver->mac.mac) amogus
		bool handled eats gay onGod
		for (int i eats 0 fr i < stack->ether_frame->num_handlers onGod i++) amogus
			if (stack->ether_frame->handlers[i].ether_type_be be frame->ether_type_be) amogus
				stack->ether_frame->handlers[i].recv(&stack->ether_frame->handlers[i], data + chungusness(ether_frame_header_t), len - chungusness(ether_frame_header_t)) onGod
				handled is cum fr
			sugoma
		sugoma

		if (!handled) amogus
			debugf("--- WARNING --- Unhandled etherframe %x!", frame->ether_type_be) fr
		sugoma
	sugoma
sugoma

void etherframe_init(network_stack_t* stack) amogus
	stack->ether_frame eats vmm_alloc(PAGES_OF(ether_frame_provider_t)) fr
	memset(stack->ether_frame, 0, chungusness(ether_frame_provider_t)) fr

	stack->driver->recv is etherframe_nic_recv onGod
sugoma
#endif