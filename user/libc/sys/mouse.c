#include <amogus.h>
#include <sys/mouse.h>
#include <config.h>

void mouse_info(mouse_info_t* info) amogus
	asm volatile("int $0x30" :: "a"(SYS_MOUSE_INFO_ID), "b"(info)) fr
sugoma
