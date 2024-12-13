#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/socket_manager.h>
#include <assert.h>
#include <scheduler/scheduler.h>
#include <stdio.h>
#include <config.h>
#ifdef NETWORK_STACK


void sys_connect_dealloc(void* resource) {
	debugf("Freeing resource %x", resource);
	socket_disconnect((socket_t*) resource);
}

cpu_registers_t* sys_sock_connect(cpu_registers_t* regs) {
	assert(regs->ebx < num_nic_drivers);

	NOSHED(
		regs->esi = socket_connect((network_stack_t*) nic_drivers[regs->ebx]->driver.driver_specific_data, regs->esi, (ip_u) regs->ecx, regs->edx)->socket_id;
	);

	resource_register_self((resource_t) {
		.dealloc = sys_connect_dealloc,
		.resource = socket_manager_find(regs->esi)
	});

	return regs;
}
#endif