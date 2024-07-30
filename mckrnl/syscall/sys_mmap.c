#include <syscall/syscalls.h>

#include <memory/vmm.h>
#include <memory/pmm.h>
#include <scheduler/scheduler.h>

cpu_registers_t* sys_mmap(cpu_registers_t* regs) {
	// debugf("sys_mmap(%x)", regs->ebx);
	
	vmm_map_page(get_self()->context, (uintptr_t) regs->ebx, (uintptr_t) pmm_alloc(), PTE_PRESENT | PTE_WRITE | PTE_USER);

	return regs;
}