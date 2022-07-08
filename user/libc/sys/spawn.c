#include <sys/spawn.h>

#include <config.h>

int spawn(const char *path, const char *argv[], const char *envp[]) {
	int pid;
	asm volatile("int $0x30" : "=S"(pid) : "a"(SYS_SPAWN), "b"(path), "c"(argv), "d"(envp));
	return pid;
}