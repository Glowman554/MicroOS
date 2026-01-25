#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/socket_manager.h>
#include <scheduler/scheduler.h>
#include <config.h>
#include <stdio.h>
#ifdef NETWORK_STACK


cpu_registers_t* sys_sock_set_local_port(cpu_registers_t* regs) {
	socket_t* socket = socket_manager_find(regs->ebx);
	if (!socket) {
		abortf(true, "sys_sock_set_local_port: invalid socket id %d", regs->ebx);
	}
	socket_set_local_port(socket, regs->ecx);

	return regs;
}
#endif