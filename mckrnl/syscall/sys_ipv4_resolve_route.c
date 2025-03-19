#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/ipv4.h>
#include <assert.h>
#include <config.h>

#ifdef NETWORK_STACK
cpu_registers_t* sys_ipv4_resolve_route(cpu_registers_t* regs) {
	assert(regs->ebx < num_nic_drivers);

    mac_u route = ipv4_resolve_route((network_stack_t*) nic_drivers[regs->ebx]->driver.driver_specific_data, (async_t*) regs->ecx, (ip_u) regs->edx);
    *((mac_u*) regs->esi) = route;

    return regs;
}
#endif