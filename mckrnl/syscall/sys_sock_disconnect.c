#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/socket_manager.h>
#include <scheduler/scheduler.h>
#include <config.h>
#include <stdio.h>
#ifdef NETWORK_STACK


cpu_registers_t* sys_sock_disconnect(cpu_registers_t* regs) {
	socket_t* socket = socket_manager_find(regs->ebx);
	if (!socket) {
		abortf("sys_sock_disconnect: invalid socket id %d", regs->ebx);
	}
	socket_disconnect(socket, (async_t*) regs->ecx);

	if (is_resolved((async_t*) regs->ecx)) {
		resource_unregister_self(socket);
	}

	return regs;
}
#endif