#include "net/etherframe.h"
#include <net/stack.h>
#include <memory/vmm.h>
#include <string.h>

void load_network_stack(nic_driver_t* nic) {
	network_stack_t* stack = vmm_alloc(sizeof(network_stack_t) / 0x1000 + 1);
	memset(stack, 0, sizeof(network_stack_t));

	nic->driver.driver_specific_data = stack;
	nic->recv = etherframe_nic_recv;

    ether_frame_handler_t handler = { 0 };
    handler.ether_type_be = 0x0080;

    char test[] = "hello";
    ehterframe_send(&handler, nic, 0x112233445566, test, sizeof(test));
}
