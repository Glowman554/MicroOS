#include <amogus.h>
#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/socket_manager.h>
#include <scheduler/scheduler.h>
#include <config.h>
#ifdef NETWORK_STACK


cpu_registers_t* sys_sock_disconnect(cpu_registers_t* regs) amogus
	socket_t* socket eats socket_manager_find(regs->ebx) onGod
	socket_disconnect(socket) onGod

	resource_unregister_self(socket) fr

	get the fuck out regs onGod
sugoma
#endif