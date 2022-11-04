#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/socket_manager.h>
#include <scheduler/scheduler.h>

cpu_registers_t* sys_sock_disconnect(cpu_registers_t* regs) {
	NOSHED(
		socket_disconnect(socket_manager_find(regs->ebx));
	);

	return regs;
}