#include <sys/env.h>

#include <config.h>

void* env(int id) {
	void* ret;
	asm volatile("int $0x30" : "=c"(ret) : "a"(SYS_ENV_ID), "b"(id));
	return ret;
}

void set_env(int id, void* value) {
	asm volatile("int $0x30" : : "a"(SYS_ENV_ID), "b"(id), "c"(value));
}