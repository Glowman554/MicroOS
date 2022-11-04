#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/icmp.h>
#include <assert.h>
#include <scheduler/scheduler.h>

cpu_registers_t* sys_icmp(cpu_registers_t* regs) {
	assert(regs->ebx < num_nic_drivers);

	NOSHED(
		regs->edx = icmp_send_echo_reqest_and_wait((network_stack_t*) nic_drivers[regs->ebx]->driver.driver_specific_data, (ip_u) regs->ecx);
	);

	return regs;
}