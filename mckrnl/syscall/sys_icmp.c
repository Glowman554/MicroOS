#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/icmp.h>
#include <stdio.h>
#include <stddef.h>
#include <config.h>
#ifdef NETWORK_STACK

cpu_registers_t* sys_icmp(cpu_registers_t* regs) {
	if (regs->ebx >= num_nic_drivers || !nic_drivers[regs->ebx]) {
		abortf("sys_icmp: invalid NIC driver index %d", regs->ebx);
	}

	network_stack_t* stack = (network_stack_t*) nic_drivers[regs->ebx]->driver.driver_specific_data;
	if (!stack) {
		abortf("sys_icmp: network stack not initialized for NIC driver index %d", regs->ebx);
	}

	icmp_send_echo_request_and_wait(stack, (async_t*) regs->ecx, (ip_u) regs->edx, *((mac_u*) regs->esi));

	return regs;
}
#endif