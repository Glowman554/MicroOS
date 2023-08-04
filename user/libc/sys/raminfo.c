#include <sys/raminfo.h>
#include <config.h>

void raminfo(uint32_t* free, uint32_t* used) {
    uint32_t _free;
    uint32_t _used;
	asm volatile("int $0x30" : "=b"(_free), "=c"(_used) : "a"(SYS_RAMINFO_ID));

    *free = _free;
    *used = _used;
}