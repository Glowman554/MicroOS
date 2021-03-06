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