#include <syscall/syscalls.h>
#include <scheduler/message.h>

cpu_registers_t* sys_message_recv(cpu_registers_t* regs) {
	uint32_t topic_id = regs->ebx;
	void* buffer = (void*) regs->ecx;
	uint32_t size = regs->edx;

	if (buffer == NULL || size == 0) {
		regs->eax = 0;
		return regs;
	}

	regs->eax = message_recv(topic_id, buffer, size);

	return regs;
}
