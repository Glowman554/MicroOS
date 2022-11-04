#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/ntp.h>
#include <assert.h>
#include <scheduler/scheduler.h>

cpu_registers_t* sys_ntp(cpu_registers_t* regs) {
	assert(regs->ebx < num_nic_drivers);

	NOSHED(
		time_t time = ntp_time((network_stack_t*) nic_drivers[regs->ebx]->driver.driver_specific_data);
		*((time_t*) regs->ecx) = time;
	);

	return regs;
}