#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/socket_manager.h>
#include <scheduler/scheduler.h>
#include <config.h>
#ifdef NETWORK_STACK

cpu_registers_t* sys_sock_recv(cpu_registers_t* regs) {
	NOSHED(
		regs->esi = socket_recv(socket_manager_find(regs->ebx), (uint8_t*) regs->ecx, regs->edx);
	);

	return regs;
}
#endif