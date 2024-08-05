#include <syscall/syscalls.h>

#include <stdio.h>
#include <stddef.h>
#include <memory/vmm.h>
#include <config.h>

syscall_handler_t* syscall_table = { 0 };
int num_syscall_handlers = 0;

void register_syscall(uint8_t syscall_id, syscall_handler_t handler) {
	debugf("Registering syscall %d with handler %p", syscall_id, handler);

	if (syscall_id > num_syscall_handlers) {
		int old_num_syscall_handlers = num_syscall_handlers;
		num_syscall_handlers = syscall_id + 1;
		syscall_table = vmm_resize(sizeof(syscall_handler_t), old_num_syscall_handlers, num_syscall_handlers, syscall_table);
	}

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
	register_syscall(SYS_VCURSOR_ID, sys_vcursor);
	register_syscall(SYS_ICMP_ID, sys_icmp);
	register_syscall(SYS_DNS_A_ID, sys_dns_a);
	register_syscall(SYS_SOCK_CONNECT_ID, sys_sock_connect);
	register_syscall(SYS_SOCK_DISCONNECT_ID, sys_sock_disconnect);
	register_syscall(SYS_SOCK_SEND_ID, sys_sock_send);
	register_syscall(SYS_SOCK_RECV_ID, sys_sock_recv);
	register_syscall(SYS_SOUND_RUN_ID, sys_sound_run);
	register_syscall(SYS_TIME_ID, sys_time);
	register_syscall(SYS_SET_COLOR_ID, sys_set_color);
	register_syscall(SYS_ASYNC_GETARRW_ID, sys_async_getarrw);
	register_syscall(SYS_VCURSOR_GET_ID, sys_vcursor_get);
	register_syscall(SYS_TASK_LIST_GET_ID, sys_task_list_get);
	register_syscall(SYS_KILL_ID, sys_kill);
	register_syscall(SYS_VPEEK_ID, sys_vpeek);
	register_syscall(SYS_RAMINFO_ID, sys_raminfo);
	register_syscall(SYS_MOUSE_INFO_ID, sys_mouse_info);
	register_syscall(SYS_TRUNCATE_ID, sys_truncate);
	register_syscall(SYS_TIME_MS_ID, sys_time_ms);
	register_syscall(SYS_SET_TERM_ID, sys_set_term);
#ifdef THREADS
	register_syscall(SYS_THREAD_ID, sys_thread);
#endif

	register_interrupt_handler(0x30, syscall_handler, NULL);
}