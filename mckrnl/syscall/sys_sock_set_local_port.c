#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/socket_manager.h>
#include <scheduler/scheduler.h>
#include <config.h>
#ifdef NETWORK_STACK


cpu_registers_t* sys_sock_set_local_port(cpu_registers_t* regs) {
	socket_set_local_port(socket_manager_find(regs->ebx), regs->ecx);

	return regs;
}
#endif