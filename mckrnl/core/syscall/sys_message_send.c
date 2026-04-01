#include <syscall/syscalls.h>
#include <scheduler/message.h>
#include <stdio.h>

cpu_registers_t* sys_message_send(cpu_registers_t* regs) {
	uint32_t topic_id = regs->ebx;
	void* buffer = (void*) regs->ecx;
	uint32_t size = regs->edx;

	if (buffer == NULL || size == 0) {
		abortf(true, "sys_message_send: invalid buffer or size");
	}

	message_send(topic_id, buffer, size);

	return regs;
}
