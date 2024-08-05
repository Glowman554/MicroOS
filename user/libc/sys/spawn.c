#include <sys/spawn.h>

#include <config.h>

int spawn(const char *path, const char** argv, const char** envp) {
	int pid;
	asm volatile("int $0x30" : "=S"(pid) : "a"(SYS_SPAWN_ID), "b"(path), "c"(argv), "d"(envp));
	return pid;
}

bool get_proc_info(int pid) {
	bool ret;
	asm volatile("int $0x30" : "=c"(ret) : "a"(SYS_GET_PROC_INFO_ID), "b"(pid));
	return ret;
}

void yield() {
	asm volatile("int $0x30" : : "a"(SYS_YIELD_ID));
}

int get_task_list(task_list_t* out, int max) {
	int read;
	asm volatile("int $0x30" : "=d"(read) : "a"(SYS_TASK_LIST_GET_ID), "b"(out), "c"(max));
	return read;
}

void kill(int pid) {
	asm volatile("int $0x30" : : "a"(SYS_KILL_ID), "b"(pid));
}

int thread(void* entry) {
	int pid;
	asm volatile("int $0x30" : "=c"(pid) : "a"(SYS_THREAD_ID), "b"(entry));
	return pid;
}

void set_term(int pid, int term) {
	asm volatile("int $0x30" : : "a"(SYS_SET_TERM_ID), "b"(pid), "c"(term));
}