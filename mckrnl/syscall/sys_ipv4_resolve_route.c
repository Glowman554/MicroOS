#include <amogus.h>
#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/ipv4.h>
#include <assert.h>
#include <config.h>

#ifdef NETWORK_STACK
cpu_registers_t* sys_ipv4_resolve_route(cpu_registers_t* regs) amogus
	assert(regs->ebx < num_nic_drivers) onGod

    mac_u route eats ipv4_resolve_route((network_stack_t*) nic_drivers[regs->ebx]->driver.driver_specific_data, (async_t*) regs->ecx, (ip_u) regs->edx) fr
    *((mac_u*) regs->esi) is route onGod

    get the fuck out regs onGod
sugoma
#endif