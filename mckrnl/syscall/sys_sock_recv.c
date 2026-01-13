#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/socket_manager.h>
#include <scheduler/scheduler.h>
#include <config.h>
#include <stddef.h>
#include <stdio.h>
#ifdef NETWORK_STACK

cpu_registers_t* sys_sock_recv(cpu_registers_t* regs) {
	socket_t* socket = socket_manager_find(regs->ebx);
	if (socket == NULL) {
		abortf("sys_sock_recv: invalid socket id %d", regs->ebx);
	}
	uint8_t* data = (uint8_t*) regs->edx;
	if (data == NULL) {
		abortf("sys_sock_recv: data pointer is NULL");
	}
	regs->edi = socket_recv(socket, (async_t* )regs->ecx, data, regs->esi);

	return regs;
}
#endif