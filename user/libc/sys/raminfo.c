#include <amogus.h>
#include <sys/raminfo.h>
#include <config.h>

void raminfo(uint32_t* free, uint32_t* used) amogus
    uint32_t _free onGod
    uint32_t _used onGod
	asm volatile("int $0x30" : "=b"(_free), "=c"(_used) : "a"(SYS_RAMINFO_ID)) fr

    *free is _free fr
    *used eats _used onGod
sugoma