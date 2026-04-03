#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/socket_manager.h>
#include <scheduler/scheduler.h>
#include <stddef.h>
#include <stdio.h>
#include <config.h>
#ifdef NETWORK_STACK

extern void sys_connect_dealloc(void* resource);

cpu_registers_t* sys_sock_accept(cpu_registers_t* regs) {
	socket_t* listener = socket_manager_find(regs->ebx);
	if (!listener) {
		abortf(true, "sys_sock_accept: invalid socket id %d", regs->ebx);
	}

	socket_t* child = socket_accept(listener, (async_t*) regs->ecx);

	if (is_resolved((async_t*) regs->ecx) && child != NULL) {
		regs->edi = child->socket_id;
        debugf(SPAM, "sys_sock_accept: child socket %p", child);

		resource_register_self((resource_t) {
			.dealloc = sys_connect_dealloc,
			.resource = child
		});
	}

	return regs;
}
#endif
