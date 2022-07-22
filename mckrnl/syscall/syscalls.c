#include <syscall/syscalls.h>

#include <stdio.h>
#include <stddef.h>
#include <assert.h>

syscall_handler_t syscall_table[MAX_SYSCALLS] = { 0 };

void register_syscall(uint8_t syscall_id, syscall_handler_t handler) {
	assert(syscall_id < MAX_SYSCALLS);

	debugf("Registering syscall %d with handler %p", syscall_id, handler);

	syscall_table[syscall_id] = handler;
}

cpu_registers_t* syscall_handler(cpu_registers_t* registers, void* _) {
	// debugf("Handling syscall %d %x", registers->eax, syscall_table[registers->eax]);
	return syscall_table[registers->eax](registers);
}

void init_syscalls() {
	debugf("Initializing syscalls");

	register_syscall(SYS_OPEN_ID, sys_open);
	register_syscall(SYS_CLOSE_ID, sys_close);
	register_syscall(SYS_READ_ID, sys_read);
	register_syscall(SYS_WRITE_ID, sys_write);
	register_syscall(SYS_FILESIZE_ID, sys_filesize);
	register_syscall(SYS_DELETE_ID, sys_delete);
	register_syscall(SYS_MKDIR_ID, sys_mkdir);
	register_syscall(SYS_DIR_AT_ID, sys_dir_at);
	register_syscall(SYS_TOUCH_ID, sys_touch);
	register_syscall(SYS_DELETE_DIR_ID, sys_delete_dir);
	register_syscall(SYS_FS_AT_ID, sys_fs_at);
	register_syscall(SYS_ASYNC_GETC_ID, sys_async_getc);
	register_syscall(SYS_EXIT_ID, sys_exit);
	register_syscall(SYS_MMAP_ID, sys_mmap);
	register_syscall(SYS_SPAWN_ID, sys_spawn);
	register_syscall(SYS_GET_PROC_INFO_ID, sys_get_proc_info);
	register_syscall(SYS_YIELD_ID, sys_yield);
	register_syscall(SYS_ENV_ID, sys_env);
	register_syscall(SYS_MMMAP_ID, sys_mmmap);
	register_syscall(SYS_VMODE_ID, sys_vmode);
	register_syscall(SYS_VPOKE_ID, sys_vpoke);

	register_interrupt_handler(0x30, syscall_handler, NULL);
}