#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/socket_manager.h>
#include <scheduler/scheduler.h>
#include <config.h>
#include <stddef.h>
#include <stdio.h>
#ifdef NETWORK_STACK


cpu_registers_t* sys_sock_send(cpu_registers_t* regs) {
	socket_t* socket = socket_manager_find(regs->ebx);
	if (socket == NULL) {
		abortf(true, "sys_sock_send: invalid socket id %d", regs->ebx);
	}
	uint8_t* data = (uint8_t*) regs->ecx;
	if (data == NULL) {
		abortf(true, "sys_sock_send: data pointer is NULL");
	}
	socket_send(socket, data, regs->edx);

	return regs;
}
#endif