#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/socket_manager.h>
#include <scheduler/scheduler.h>
#include <config.h>
#ifdef NETWORK_STACK

cpu_registers_t* sys_sock_recv(cpu_registers_t* regs) {
	regs->edi = socket_recv(socket_manager_find(regs->ebx), (async_t* )regs->ecx, (uint8_t*) regs->edx, regs->esi);

	return regs;
}
#endif