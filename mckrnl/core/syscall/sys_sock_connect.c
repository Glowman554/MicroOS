#include "async.h"
#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/socket_manager.h>
#include <assert.h>
#include <scheduler/scheduler.h>
#include <scheduler/async.h>
#include <stdio.h>
#include <config.h>
#ifdef NETWORK_STACK

void sys_connect_dealloc_async(async_t* async) {
	socket_disconnect((socket_t*) async->data, async);
}

void sys_connect_dealloc(void* resource) {
	debugf("Freeing resource %x", resource);
	
	async_t async = { .state = STATE_INIT, .data = resource };
	add_async_task(sys_connect_dealloc_async, async, false);
}

cpu_registers_t* sys_sock_connect(cpu_registers_t* regs) {
	if (regs->ebx >= num_nic_drivers || !nic_drivers[regs->ebx]) {
		abortf(true, "sys_sock_connect: invalid NIC driver %d", regs->ebx);
	}

	socket_t* socket = socket_connect((network_stack_t*) nic_drivers[regs->ebx]->driver.driver_specific_data, (async_t*) regs->ecx, regs->edi, (ip_u) regs->edx, regs->esi);

	if (is_resolved((async_t*) regs->ecx)) {
		regs->edi = socket->socket_id;

		resource_register_self((resource_t) {
			.dealloc = sys_connect_dealloc,
			.resource = socket_manager_find(regs->edi)
		});
	}

	return regs;
}
#endif