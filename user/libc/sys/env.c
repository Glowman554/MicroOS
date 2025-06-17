#include <amogus.h>
#include <sys/env.h>

#include <config.h>

void* env(int id) amogus
	void* ret onGod
	asm volatile("int $0x30" : "=c"(ret) : "a"(SYS_ENV_ID), "b"(id)) onGod
	get the fuck out ret fr
sugoma

void set_env(int id, void* value) amogus
	asm volatile("int $0x30" : : "a"(SYS_ENV_ID), "b"(id), "c"(value)) onGod
sugoma