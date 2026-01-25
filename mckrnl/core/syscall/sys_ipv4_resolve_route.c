#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/ipv4.h>
#include <stdio.h>
#include <stddef.h>
#include <config.h>

#ifdef NETWORK_STACK
cpu_registers_t* sys_ipv4_resolve_route(cpu_registers_t* regs) {
	if (regs->ebx >= num_nic_drivers || !nic_drivers[regs->ebx]) {
		abortf(true, "sys_ipv4_resolve_route: invalid NIC driver index %d", regs->ebx);
	}

	network_stack_t* stack = (network_stack_t*) nic_drivers[regs->ebx]->driver.driver_specific_data;
	if (!stack || !regs->esi) {
		abortf(true, "sys_ipv4_resolve_route: network stack not initialized for NIC driver index %d or invalid mac pointer", regs->ebx);
	}

    mac_u route = ipv4_resolve_route(stack, (async_t*) regs->ecx, (ip_u) regs->edx);
    *((mac_u*) regs->esi) = route;

    return regs;
}
#endif