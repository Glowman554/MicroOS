#include <sys/spawn.h>

#include <config.h>

int spawn(const char *path, const char** argv, const char** envp) {
	int pid;
	asm volatile("int $0x30" : "=S"(pid) : "a"(SYS_SPAWN_ID), "b"(path), "c"(argv), "d"(envp), "S"(0), "D"(0));
	return pid;
}

int spawn_with_pipes(const char *path, const char** argv, const char** envp, bool enable_stdout_pipe, bool enable_stdin_pipe) {
	int pid;
	asm volatile("int $0x30" : "=S"(pid) : "a"(SYS_SPAWN_ID), "b"(path), "c"(argv), "d"(envp), "S"(enable_stdout_pipe), "D"(enable_stdin_pipe));
	return pid;
}

size_t pipe_read_stdout(int pid, void* buffer, size_t max_size) {
	size_t bytes_read;
	asm volatile("int $0x30" : "=S"(bytes_read) : "a"(SYS_PIPE_READ_STDOUT_ID), "b"(pid), "c"(buffer), "d"(max_size));
	return bytes_read;
}

void pipe_write_stdin(int pid, const void* buffer, size_t size) {
	asm volatile("int $0x30" : : "a"(SYS_PIPE_WRITE_STDIN_ID), "b"(pid), "c"(buffer), "d"(size));
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