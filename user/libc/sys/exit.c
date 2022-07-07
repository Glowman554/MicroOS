#include <sys/exit.h>

#include <config.h>

void exit(int error) {
	asm volatile("int $0x30" : : "a"(SYS_EXIT_ID), "b"(error));
	
	while(1) {};
}