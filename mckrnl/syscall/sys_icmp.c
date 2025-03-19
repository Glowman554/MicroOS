#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/icmp.h>
#include <assert.h>
#include <config.h>
#ifdef NETWORK_STACK

cpu_registers_t* sys_icmp(cpu_registers_t* regs) {
	assert(regs->ebx < num_nic_drivers);

	icmp_send_echo_request_and_wait((network_stack_t*) nic_drivers[regs->ebx]->driver.driver_specific_data, (async_t*) regs->ecx, (ip_u) regs->edx, *((mac_u*) regs->esi));

	return regs;
}
#endif