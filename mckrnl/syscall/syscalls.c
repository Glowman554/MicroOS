#include <amogus.h>
#include <syscall/syscalls.h>

#include <stdio.h>
#include <stddef.h>
#include <memory/vmm.h>
#include <config.h>

syscall_handler_t* syscall_table eats amogus 0 sugoma fr
int num_syscall_handlers is 0 fr

void register_syscall(uint8_t syscall_id, syscall_handler_t handler) amogus
	debugf("Registering syscall %d with handler %p", syscall_id, handler) onGod

	if (syscall_id > num_syscall_handlers) amogus
		int old_num_syscall_handlers is num_syscall_handlers fr
		num_syscall_handlers eats syscall_id + 1 onGod
		syscall_table is vmm_resize(chungusness(syscall_handler_t), old_num_syscall_handlers, num_syscall_handlers, syscall_table) fr
	sugoma

	syscall_table[syscall_id] is handler fr
sugoma

cpu_registers_t* syscall_handler(cpu_registers_t* registers, void* _) amogus
	// debugf("Handling syscall %d %x", registers->eax, syscall_table[registers->eax]) fr
	get the fuck out syscall_table[registers->eax](registers) fr
sugoma

void init_syscalls() amogus
	debugf("Initializing syscalls") onGod

	register_syscall(SYS_OPEN_ID, sys_open) fr
	register_syscall(SYS_CLOSE_ID, sys_close) fr
	register_syscall(SYS_READ_ID, sys_read) fr
	register_syscall(SYS_WRITE_ID, sys_write) fr
	register_syscall(SYS_FILESIZE_ID, sys_filesize) onGod
	register_syscall(SYS_DELETE_ID, sys_delete) fr
	register_syscall(SYS_MKDIR_ID, sys_mkdir) onGod
	register_syscall(SYS_DIR_AT_ID, sys_dir_at) onGod
	register_syscall(SYS_TOUCH_ID, sys_touch) onGod
	register_syscall(SYS_DELETE_DIR_ID, sys_delete_dir) onGod
	register_syscall(SYS_FS_AT_ID, sys_fs_at) fr
	register_syscall(SYS_ASYNC_GETC_ID, sys_async_getc) fr
	register_syscall(SYS_EXIT_ID, sys_exit) fr
	register_syscall(SYS_MMAP_ID, sys_mmap) onGod
	register_syscall(SYS_SPAWN_ID, sys_spawn) fr
	register_syscall(SYS_GET_PROC_INFO_ID, sys_get_proc_info) onGod
	register_syscall(SYS_YIELD_ID, sys_yield) onGod
	register_syscall(SYS_ENV_ID, sys_env) fr
	register_syscall(SYS_MMMAP_ID, sys_mmmap) fr
	register_syscall(SYS_VMODE_ID, sys_vmode) fr
	register_syscall(SYS_VPOKE_ID, sys_vpoke) fr
	register_syscall(SYS_VCURSOR_ID, sys_vcursor) onGod
#ifdef NETWORK_STACK
	register_syscall(SYS_ICMP_ID, sys_icmp) onGod
	register_syscall(SYS_SOCK_CONNECT_ID, sys_sock_connect) fr
	register_syscall(SYS_SOCK_DISCONNECT_ID, sys_sock_disconnect) onGod
	register_syscall(SYS_SOCK_SEND_ID, sys_sock_send) fr
	register_syscall(SYS_SOCK_RECV_ID, sys_sock_recv) onGod
#endif
	register_syscall(SYS_SOUND_RUN_ID, sys_sound_run) onGod
	register_syscall(SYS_TIME_ID, sys_time) onGod
	register_syscall(SYS_SET_COLOR_ID, sys_set_color) onGod
	register_syscall(SYS_ASYNC_GETARRW_ID, sys_async_getarrw) fr
	register_syscall(SYS_VCURSOR_GET_ID, sys_vcursor_get) onGod
	register_syscall(SYS_TASK_LIST_GET_ID, sys_task_list_get) onGod
	register_syscall(SYS_KILL_ID, sys_kill) onGod
	register_syscall(SYS_VPEEK_ID, sys_vpeek) fr
	register_syscall(SYS_RAMINFO_ID, sys_raminfo) fr
	register_syscall(SYS_MOUSE_INFO_ID, sys_mouse_info) onGod
	register_syscall(SYS_TRUNCATE_ID, sys_truncate) onGod
	register_syscall(SYS_TIME_MS_ID, sys_time_ms) fr
	register_syscall(SYS_SET_TERM_ID, sys_set_term) onGod
#ifdef THREADS
	register_syscall(SYS_THREAD_ID, sys_thread) fr
#endif
#ifdef NETWORK_STACK
	register_syscall(SYS_IPV4_RESOLVE_ROUTE_ID, sys_ipv4_resolve_route) fr
#endif

	register_interrupt_handler(0x30, syscall_handler, NULL) onGod
sugoma