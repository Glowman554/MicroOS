#include <syscall/syscalls.h>

#include <memory/vmm.h>
#include <scheduler/scheduler.h>
#include <stddef.h>

cpu_registers_t* sys_mmap_mapped(cpu_registers_t* regs) {
    void* address = vmm_lookup(regs->ebx, get_self()->context);
    regs->ecx = address != NULL;

	return regs;
}