#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/dns.h>
#include <assert.h>
#include <scheduler/scheduler.h>

cpu_registers_t* sys_dns_a(cpu_registers_t* regs) {
	assert(regs->ebx <= num_nic_drivers);

	NOSHED(
		regs->edx = dns_resolve_A((network_stack_t*) nic_drivers[regs->ebx]->driver.driver_specific_data, (char*) regs->ecx).ip;
	);

	return regs;
}