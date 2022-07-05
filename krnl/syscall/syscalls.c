#include <syscall/syscalls.h>

#include <stdio.h>
#include <stddef.h>

syscall_handler_t syscall_table[MAX_SYSCALLS] = { 0 };

void register_syscall(uint8_t syscall_id, syscall_handler_t handler) {
	debugf("Registering syscall %d with handler %p", syscall_id, handler);

	syscall_table[syscall_id] = handler;
}

cpu_registers_t* syscall_handler(cpu_registers_t* registers) {
	// debugf("Handling syscall %d", registers->eax);
	return syscall_table[registers->eax](registers);
}

void init_syscalls() {
	debugf("Initializing syscalls");

	register_syscall(SYS_PUTC_ID, sys_putc);

	register_interrupt_handler(0x30, syscall_handler);
}