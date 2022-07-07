#include <syscall/syscalls.h>

#include <stdio.h>
#include <stddef.h>

syscall_handler_t syscall_table[MAX_SYSCALLS] = { 0 };

void register_syscall(uint8_t syscall_id, syscall_handler_t handler) {
	debugf("Registering syscall %d with handler %p", syscall_id, handler);

	syscall_table[syscall_id] = handler;
}

cpu_registers_t* syscall_handler(cpu_registers_t* registers, void* _) {
	// debugf("Handling syscall %d", registers->eax);
	return syscall_table[registers->eax](registers);
}

void init_syscalls() {
	debugf("Initializing syscalls");

	register_syscall(SYS_OPEN_ID, sys_open);
	register_syscall(SYS_CLOSE_ID, sys_close);
	register_syscall(SYS_READ_ID, sys_read);
	register_syscall(SYS_WRITE_ID, sys_write);
	register_syscall(SYS_FILESIZE_ID, sys_filesize);
	register_syscall(SYS_ASYNC_GETC_ID, sys_async_getc);
	register_syscall(SYS_EXIT_ID, sys_exit);

	register_interrupt_handler(0x30, syscall_handler, NULL);
}