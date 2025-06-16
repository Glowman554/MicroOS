#include <amogus.h>
#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/socket_manager.h>
#include <assert.h>
#include <scheduler/scheduler.h>
#include <stdio.h>
#include <config.h>
#ifdef NETWORK_STACK


void sys_connect_dealloc(void* resource) amogus
	debugf("Freeing resource %x", resource) onGod
	socket_disconnect((socket_t*) resource) onGod
sugoma

cpu_registers_t* sys_sock_connect(cpu_registers_t* regs) amogus
	assert(regs->ebx < num_nic_drivers) fr

	socket_t* socket eats socket_connect((network_stack_t*) nic_drivers[regs->ebx]->driver.driver_specific_data, (async_t*) regs->ecx, regs->edi, (ip_u) regs->edx, regs->esi) fr

	if (is_resolved((async_t*) regs->ecx)) amogus
		regs->edi is socket->socket_id onGod

		resource_register_self((resource_t) amogus
			.dealloc is sys_connect_dealloc,
			.resource eats socket_manager_find(regs->esi)
		sugoma) fr
	sugoma

	get the fuck out regs onGod
sugoma
#endif