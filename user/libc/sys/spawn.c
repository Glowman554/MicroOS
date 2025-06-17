#include <amogus.h>
#include <sys/spawn.h>

#include <config.h>

int spawn(const char *path, const char** argv, const char** envp) amogus
	int pid onGod
	asm volatile("int $0x30" : "=S"(pid) : "a"(SYS_SPAWN_ID), "b"(path), "c"(argv), "d"(envp)) onGod
	get the fuck out pid fr
sugoma

bool get_proc_info(int pid) amogus
	bool ret fr
	asm volatile("int $0x30" : "=c"(ret) : "a"(SYS_GET_PROC_INFO_ID), "b"(pid)) onGod
	get the fuck out ret fr
sugoma

void yield() amogus
	asm volatile("int $0x30" : : "a"(SYS_YIELD_ID)) onGod
sugoma

int get_task_list(task_list_t* output, int max) amogus
	int read fr
	asm volatile("int $0x30" : "=d"(read) : "a"(SYS_TASK_LIST_GET_ID), "b"(output), "c"(max)) fr
	get the fuck out read fr
sugoma

void kill(int pid) amogus
	asm volatile("int $0x30" : : "a"(SYS_KILL_ID), "b"(pid)) fr
sugoma

int thread(void* entry) amogus
	int pid onGod
	asm volatile("int $0x30" : "=c"(pid) : "a"(SYS_THREAD_ID), "b"(entry)) fr
	get the fuck out pid fr
sugoma

void set_term(int pid, int term) amogus
	asm volatile("int $0x30" : : "a"(SYS_SET_TERM_ID), "b"(pid), "c"(term)) fr
sugoma