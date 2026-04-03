#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/socket_manager.h>
#include <scheduler/scheduler.h>
#include <stddef.h>
#include <stdio.h>
#include <config.h>
#ifdef NETWORK_STACK

extern void sys_connect_dealloc(void* resource);

cpu_registers_t* sys_sock_listen(cpu_registers_t* regs) {
	if (regs->ebx >= num_nic_drivers || !nic_drivers[regs->ebx]) {
		abortf(true, "sys_sock_listen: invalid NIC driver %d", regs->ebx);
	}

	socket_t* socket = socket_listen(
		(network_stack_t*) nic_drivers[regs->ebx]->driver.driver_specific_data,
		regs->ecx, regs->edx);

	if (socket != NULL) {
		regs->edi = socket->socket_id;

		resource_register_self((resource_t) {
			.dealloc = sys_connect_dealloc,
			.resource = socket
		});
	} else {
		regs->edi = -1;
	}

	return regs;
}
#endif
