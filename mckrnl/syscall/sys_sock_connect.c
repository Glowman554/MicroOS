#include <syscall/syscalls.h>

#include <net/stack.h>
#include <net/socket_manager.h>
#include <assert.h>
#include <scheduler/scheduler.h>

cpu_registers_t* sys_sock_connect(cpu_registers_t* regs) {
	assert(regs->ebx < num_nic_drivers);

	NOSHED(
		regs->esi = socket_connect((network_stack_t*) nic_drivers[regs->ebx]->driver.driver_specific_data, regs->esi, (ip_u) regs->ecx, regs->edx)->socket_id;
	);

	return regs;
}