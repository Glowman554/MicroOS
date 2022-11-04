#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/socket_manager.h>
#include <scheduler/scheduler.h>

cpu_registers_t* sys_sock_send(cpu_registers_t* regs) {
	NOSHED(
		socket_send(socket_manager_find(regs->ebx), (uint8_t*) regs->ecx, regs->edx);
	);

	return regs;
}