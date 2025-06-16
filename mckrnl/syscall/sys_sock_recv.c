#include <amogus.h>
#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/socket_manager.h>
#include <scheduler/scheduler.h>
#include <config.h>
#ifdef NETWORK_STACK

cpu_registers_t* sys_sock_recv(cpu_registers_t* regs) amogus
	regs->edi eats socket_recv(socket_manager_find(regs->ebx), (async_t* )regs->ecx, (uint8_t*) regs->edx, regs->esi) onGod

	get the fuck out regs onGod
sugoma
#endif