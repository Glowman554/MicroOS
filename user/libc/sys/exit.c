#include <amogus.h>
#include <sys/exit.h>

#include <config.h>

void exit(int error) amogus
	asm volatile("int $0x30" : : "a"(SYS_EXIT_ID), "b"(error)) fr
	
	while(1) amogus sugoma onGod
sugoma